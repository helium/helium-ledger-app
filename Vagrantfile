# -*- mode: ruby -*-
# vi: set ft=ruby :

# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  # Every Vagrant development environment requires a box. You can search for
  # boxes at https://vagrantcloud.com/search.
  config.vm.box = "hashicorp/bionic64"

  # Disable automatic box update checking. If you disable this, then
  # boxes will only be checked for updates when the user runs
  # `vagrant box outdated`. This is not recommended.
  # config.vm.box_check_update = false

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. In the example below,
  # accessing "localhost:8080" will access port 80 on the guest machine.
  # NOTE: This will enable public access to the opened port
  # config.vm.network "forwarded_port", guest: 80, host: 8080

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine and only allow access
  # via 127.0.0.1 to disable public access
  # config.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"

  # Create a private network, which allows host-only access to the machine
  # using a specific IP.
  # config.vm.network "private_network", ip: "192.168.33.10"

  # Create a public network, which generally matched to bridged network.
  # Bridged networks make the machine appear as another physical device on
  # your network.
  # config.vm.network "public_network"

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  #config.vm.synced_folder "../gcc-arm-none-eabi-5_3-2016q1", "/bolos-env/gcc-arm-none-eabi-5_3-2016q1"
  #config.vm.synced_folder "../clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04", "/bolos-env/clang-arm-fropi"

  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  # Example for VirtualBox:
  #
  config.vm.provider "virtualbox" do |vb|
  #   # Display the VirtualBox GUI when booting the machine
  #   vb.gui = true
  #
  #   # Customize the amount of memory on the VM:
  #   vb.memory = "1024"
  vb.customize ["modifyvm", :id, "--usb", "on"]
  vb.customize ["modifyvm", :id, "--usbehci", "on"]
  vb.customize ["usbfilter", "add", "0",
    "--target", :id,
    "--name", "Ledger Nano S",
    "--manufacturer", "Ledger",
    "--product", "Nano S"]
  end
  #
  # View the documentation for the provider you are using for more
  # information on available options.

  # Enable provisioning with a shell script. Additional provisioners such as
  # Ansible, Chef, Docker, Puppet and Salt are also available. Please see the
  # documentation for more information about their specific syntax and use.
  config.vm.provision "shell", inline: <<-SHELL
    set -ex

    apt update
    apt install -y gcc-multilib libudev-dev libusb-1.0-0-dev pkg-config python3-venv python3-pip python3-pil

    # Minimize the terminal mess wget is about to make
    echo "progress = bar:force:noscroll" > ~/.wgetrc

    # Load udev rules
    source /vagrant/udev.sh

    # Commence workaround for vbox share dirs not supportting hard links...
    # 1) Temporarily setup our "dev-env" dir in ~vagrant
    source /vagrant/prepare-devenv.sh s
    # 2) Convert hard links to duplicates of their source file
    # NOTE: Link counts may need updated should Ledger update their toolchain
    find dev-env/CC/others/gcc-arm-none-eabi-5_3-2016q1/ -type f -\\( -links 2 -o -links 3 -o -links 4 -\\) -exec sed -i ';;' '{}' \\;
    # 3) Move "dev-env" dir into the source root, where it's expected to be
    mv dev-env /vagrant
    # 4) Reinstall the python3 venv, since all of the paths are wrong
    cd /vagrant
    rm -rf dev-env/ledger_py3
    # NOTE: The next block needs to be kept in sync with prepare-devenv.sh
    python3 -m venv dev-env/ledger_py3
    source dev-env/ledger_py3/bin/activate
    pip install wheel
    pip install ledgerblue
    # End hard link work around

    # Restore wget's mess making ability
    rm ~/.wgetrc

    # Setup env for Ledger S SDK.
    # NOTE: We'll need to do something fancier (or lazier) when we add Ledger X support
    echo -e "\nsource /vagrant/prepare-devenv.sh s" >> ~vagrant/.bashrc
  SHELL

  # cd to /vagrant when "vagrant ssh"
  config.ssh.extra_args = ["-t", "cd /vagrant; bash --login"]
end
