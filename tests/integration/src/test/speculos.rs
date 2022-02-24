use ledger_transport::{exchange::Exchange as LedgerTransport, TransportTcp};
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::collections::HashMap;
use std::env;
use std::process::Stdio;
use tokio::io::AsyncBufReadExt;
use tokio::io::BufReader;
use tokio::process::Command;

pub struct Speculos {
    child: tokio::process::Child,
    reader: tokio::io::Lines<tokio::io::BufReader<tokio::process::ChildStderr>>,
    // I had trouble closing and opening speculos
    // so we spin them up on different ports
    apdu_port: u16,
    api_port: u16,
}

pub enum Device {
    NanoX,
    NanoS,
}

impl Speculos {
    pub async fn new(
        device: Device,
        apdu_port: u16,
        api_port: u16,
    ) -> Result<Speculos, Box<dyn std::error::Error>> {
        let speculos_dir = env::var("SPECULOS_DIR").expect("SPECULOS_DIR must be set");
        let speculos = format!("{speculos_dir}/speculos.py");

        let build_dir = env::var("BUILD_DIR").expect("BUILD_DIR must be set");
        let test_exec = format!("{build_dir}/bin/app.elf");

        let mut cmd = Command::new(&speculos);

        cmd.args([
            "--model",
            match device {
                Device::NanoX => "nanox",
                Device::NanoS => "nanos",
            },
        ])
        .args(["--apdu-port", apdu_port.to_string().as_str()])
        .args(["--api-port", api_port.to_string().as_str()])
        .args(["--display", "headless"])
        .arg(&test_exec);

        // for some reason, speculos outputs some regular stuff on stderr
        cmd.stdout(Stdio::piped());
        cmd.stderr(Stdio::piped());

        let mut child = cmd.spawn().expect("failed to spawn command");

        // we aren't interested in stdout, but taking it keeps it from going to the terminal
        let _stdout = child
            .stdout
            .take()
            .expect("child did not have a handle to stdout");
        let stderr = child
            .stderr
            .take()
            .expect("child did not have a handle to stderr");

        let reader = BufReader::new(stderr).lines();

        let mut speculos = Speculos {
            child,
            reader,
            api_port,
            apdu_port,
        };
        while let Some(line) = speculos.reader.next_line().await? {
            println!("{line}");
            // once we get INIT START, the app is initialized and ready for testing
            if line.contains("Press CTRL+C to quit") {
                let text = speculos
                    .collect_event_text(match device {
                        Device::NanoX => 2,
                        Device::NanoS => 1,
                    })
                    .await?;
                println!("{text}");
                assert!("Waiting for commands..." == text);
                return Ok(speculos);
            }
        }
        speculos.child.kill().await?;

        panic!("Trouble starting speculos");
    }

    pub async fn transport(&self) -> Result<Box<dyn LedgerTransport>, Box<dyn std::error::Error>> {
        use std::net::{IpAddr, Ipv4Addr, SocketAddr};
        let socket = SocketAddr::new(IpAddr::V4(Ipv4Addr::new(127, 0, 0, 1)), self.apdu_port);
        Ok(Box::new(TransportTcp::new(socket).await?))
    }

    pub async fn clear_events(&self) -> Result<(), Box<dyn std::error::Error>> {
        let client = reqwest::Client::new();
        let _response = client
            .delete(format!("http://127.0.0.1:{}/events", self.api_port))
            .send()
            .await?;
        Ok(())
    }

    pub async fn click_right(&self) -> Result<(), Box<dyn std::error::Error>> {
        let mut cmd = HashMap::new();
        cmd.insert("action", "press-and-release");
        let client = reqwest::Client::new();
        let _response = client
            .post(format!("http://127.0.0.1:{}/button/right", self.api_port))
            .json(&cmd)
            .send()
            .await?;
        Ok(())
    }

    pub async fn click_both(&self) -> Result<(), Box<dyn std::error::Error>> {
        let mut cmd = HashMap::new();
        cmd.insert("action", "press-and-release");
        let client = reqwest::Client::new();
        let _response = client
            .post(format!("http://127.0.0.1:{}/button/both", self.api_port))
            .json(&cmd)
            .send()
            .await?;
        Ok(())
    }

    pub async fn collect_event_text(
        &self,
        num_events: usize,
    ) -> Result<String, Box<dyn std::error::Error>> {
        let mut events = self.get_events().await.expect("Error getting events");
        while events.len() < num_events {
            events = self.get_events().await.expect("Error getting events");
        }
        self.clear_events().await?;
        Ok(get_event_text(events))
    }

    pub async fn get_events(&self) -> Result<Vec<Event>, Box<dyn std::error::Error>> {
        let client = reqwest::Client::new();
        let response = client
            .get(format!("http://127.0.0.1:{}/events", self.api_port))
            .send()
            .await?;
        let text = response.text().await?;

        #[derive(Deserialize, Debug)]
        pub struct Response {
            events: Vec<Event>,
        }
        let response: Response = serde_json::from_str(&text)?;
        Ok(response.events)
    }

    async fn set_automation(&self, rules: Vec<Rule>) -> Result<(), Box<dyn std::error::Error>> {
        let automation = Automation { version: 1, rules };
        let client = reqwest::Client::new();
        client
            .post(format!("http://127.0.0.1:{}/automation", self.api_port))
            .json(&automation)
            .send()
            .await?;
        Ok(())
    }

    pub async fn shutdown(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        self.set_automation(vec![Rule::default_actions(vec![vec![Value::String(
            "exit".to_string(),
        )]])])
        .await?;
        self.child.kill().await?;
        while let Some(_line) = self.reader.next_line().await? {}
        Ok(())
    }
}

#[derive(Serialize)]
struct Automation {
    version: usize,
    rules: Vec<Rule>,
}

#[derive(Serialize)]
struct Rule {
    actions: Vec<Vec<serde_json::Value>>,
}

impl Rule {
    fn default_actions(actions: Vec<Vec<serde_json::Value>>) -> Rule {
        Rule { actions }
    }
}

#[derive(Deserialize, Debug)]
pub struct Event {
    pub text: Option<String>,
    pub x: Option<usize>,
    pub y: Option<usize>,
}

fn get_event_text(mut events: Vec<Event>) -> String {
    let mut ret = String::default();
    events.reverse();
    let mut first = true;
    while let Some(event) = events.pop() {
        if let Some(text) = event.text {
            if !first {
                ret.push(' ');
            }
            ret.push_str(&text);
            first = false;
        } else {
            assert!(false);
        }
    }
    ret
    // return a string just to make compiler happy
}
