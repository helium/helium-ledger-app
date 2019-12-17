/*******************************************************************************
*
*  (c) 2016 Ledger
*  (c) 2018 Nebulous
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

// This code also serves as a walkthrough for writing your own Ledger Nano S
// app. Begin by reading this file top-to-bottom, and proceed to the next file
// when directed. It is recommended that you install this app on your Nano S
// so that you can see how each section of code maps to real-world behavior.
// This also allows you to experiment by modifying the code and observing the
// effect on the app.
//
// I'll begin by describing the high-level architecture of the app. The entry
// point is this file, main.c, which initializes the app and runs the APDU
// request/response loop. The loop reads APDU packets from the computer, which
// instructs it to run various commands. The Sia app supports three commands,
// each defined in a separate file: getPublicKey, signHash, and calcTxnHash.
// These each make use of Sia-specific functions, which are defined in sia.c.
// Finally, some global variables and helper functions are declared in ux.h.
//
// Each command consists of a command handler and a set of screens. Each
// screen has an associated set of elements that can be rendered, a
// preprocessor that controls which elements are rendered, and a button
// handler that processes user input. The command handler is called whenever
// sia_main receives an APDU requesting that command, and is responsible for
// displaying the first screen of the command. Control flow then moves to the
// button handler for that screen, which selects the next screen to display
// based on which button was pressed. Button handlers are also responsible for
// sending APDU replies back to the computer.
//
// The control flow can be a little confusing to understand, because the
// button handler isn't really on the "main execution path" -- it's only
// called via interrupt, typically while execution is blocked on an
// io_exchange call. (In general, it is instructive to think of io_exchange as
// the *only* call that can block.) io_exchange exchanges APDU packets with
// the computer: first it sends a response packet, then it receives a request
// packet. This ordering may seem strange, but it makes sense when you
// consider that the Nano S has to do work in between receiving a command and
// replying to it. Thus, the packet sent by io_exchange is a *response* to the
// previous request, and the packet received is the next request.
//
// But there's a problem with this flow: in most cases, we can't respond to
// the command request until we've received some user input, e.g. approving a
// signature. If io_exchange is the only call that blocks, how can we tell it
// to wait for user input? The answer is a special flag, IO_ASYNC_REPLY. When
// io_exchange is called with this flag, it blocks, but it doesn't send a
// response; instead, it just waits for a new request. Later on, we make a
// separate call to io_exchange, this time with the IO_RETURN_AFTER_TX flag.
// This call sends the response, and then returns immediately without waiting
// for the next request. Visually, it is clear that these flags have opposite
// effects on io_exchange:
//
//                                      ----Time--->
//    io_exchange:        [---Send Response---|---Wait for Request---]
//    IO_ASYNC_REPLY:                           ^Only do this part^
//    IO_RETURN_AFTER_TX:  ^Only do this part^
//
// So a typical command flow looks something like this. We start in sia_main,
// which is an infinite loop that starts by calling io_exchange. It receives
// an APDU request from the computer and calls the associated command handler.
// The handler displays a screen, e.g. "Generate address?", and sets the
// IO_ASYNC_REPLY flag before returning. Control returns to sia_main, which
// loops around and calls io_exchange again; due to the flag, it now blocks.
// Everything is frozen until the user decides which button to press. When
// they eventually press the "Approve" button, the button handler jumps into
// action. It generates the address, constructs a response APDU containing
// that address, calls io_exchange with IO_RETURN_AFTER_TX, and redisplays the
// main menu. When a new command arrives, it will be received by the blocked
// io_exchange in sia_main.
//
// More complex commands may require multiple requests and responses. There
// are two approaches to handling this. One approach is to treat each command
// handler as a self-contained unit: that is, the main loop should only handle
// the *first* request for a given command. Subsequent requests are handled by
// additional io_exchange calls within the command handler. The other approach
// is to let the main loop handle all requests, and design the handlers so
// that they can "pick up where they left off." Both designs have tradeoffs.
// In the Sia app, the only handler that requires multiple requests is
// calcTxnHash, and it takes the latter approach.
//
// On the other end of the spectrum, there are simple commands that do not
// require any user input. Many Nano S apps have a "getVersion" command that
// replies to the computer with the app's version. In this case, it is
// sufficient for the command handler to prepare the response APDU and allow
// the main loop to send it immediately, without setting IO_ASYNC_REPLY.
//
// The important things to remember are:
// - io_exchange is the only blocking call
// - the main loop invokes command handlers, which display screens and set button handlers
// - button handlers switch between screens and reply to the computer

#include <stdint.h>
#include <stdbool.h>
#include <os_io_seproxyhal.h>
#include "glyphs.h"
#include "helium.h"
#include "ux.h"

// You may notice that this file includes blake2b.h despite doing no hashing.
// This is because the Sia app uses the Plan 9 convention for header files:
// header files may not #include other header files. This file needs ux.h, but
// ux.h depends on helium.h, which depends on blake2b.h; so all three must be
// included before we can include ux.h. Feel free to use the more conventional
// #ifndef guards in your own app.

// These are global variables declared in ux.h. They can't be defined there
// because multiple files include ux.h; they need to be defined in exactly one
// place. See ux.h for their descriptions.
commandContext global;
ux_state_t ux;

// Here we define the main menu, using the Ledger-provided menu API. This menu
// turns out to be fairly unimportant for Nano S apps, since commands are sent
// by the computer instead of being initiated by the user. It typically just
// contains an idle screen and a version screen.

// This is a forward declaration, since menu_about needs to reference
// menu_main.
static const ux_menu_entry_t menu_main[];

static const ux_menu_entry_t menu_about[] = {
	// I won't bother describing how menus work in detail, since it's fairly
	// self-evident and not very useful; but to save you some trouble, this
	// first element is defined with explicit fields, so you can see what they
	// all are.
	{
		.menu     = NULL,       // another menu entry, displayed when this item is "entered"
		.callback = NULL,       // a function that takes a userid, called when this item is entered
		.userid   = 0,          // a custom identifier, helpful for implementing custom menu behavior
		.icon     = NULL,       // the glyph displayed next to the item text
		.line1    = "Version",  // the first line of text
		.line2    = APPVERSION, // the second line of text; if NULL, line1 will be vertically centered
		.text_x   = 0,          // the x offset of the lines of text; only used if non-zero
		.icon_x   = 0,          // the x offset of the icon; only used if non-zero
	},
	// This element references a custom glyph, C_icon_back. This glyph is
	// defined in glyphs.c, which was generated by the Makefile from the
	// corresponding .gif file in the glyphs/ folder. If you drop your own
	// .gif files into this folder and run make, they will likewise become
	// available for use in your app. The SDK also defines a few built-in
	// icons, such as BAGL_GLYPH_ICON_CHECK, which you'll see in the screen
	// definitions later on.
	{menu_main, NULL, 0, &C_icon_back, "Back", NULL, 61, 40},
	UX_MENU_END,
};

static const ux_menu_entry_t menu_main[] = {
	{NULL, NULL, 0, NULL, "Waiting for", "commands...", 0, 0},
	{menu_about, NULL, 0, NULL, "About", NULL, 0, 0},
	{NULL, os_sched_exit, 0, &C_icon_dashboard, "Quit app", NULL, 50, 29},
	UX_MENU_END,
};

// ui_idle displays the main menu. Note that your app isn't required to use a
// menu as its idle screen; you can define your own completely custom screen.
void ui_idle(void) {
	// The first argument is the starting index within menu_main, and the last
	// argument is a preprocessor; I've never seen an app that uses either
	// argument.
	UX_MENU_DISPLAY(0, menu_main, NULL);
}

// io_exchange_with_code is a helper function for sending response APDUs from
// button handlers. Note that the IO_RETURN_AFTER_TX flag is set. 'tx' is the
// conventional name for the size of the response APDU, i.e. the write-offset
// within G_io_apdu_buffer.
void io_exchange_with_code(uint16_t code, uint16_t tx) {
	G_io_apdu_buffer[tx++] = code >> 8;
	G_io_apdu_buffer[tx++] = code & 0xFF;
	io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
}

// The APDU protocol uses a single-byte instruction code (INS) to specify
// which command should be executed. We'll use this code to dispatch on a
// table of function pointers.
#define INS_GET_VERSION    0x01
#define INS_GET_PUBLIC_KEY 0x02
#define INS_SIGN_PAYMENT_TXN   0x08

// This is the function signature for a command handler. 'flags' and 'tx' are
// out-parameters that will control the behavior of the next io_exchange call
// in sia_main. It's common to set *flags |= IO_ASYNC_REPLY, but tx is
// typically unused unless the handler is immediately sending a response APDU.
typedef void handler_fn_t(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx);

handler_fn_t handle_get_version;
handler_fn_t handle_get_public_key;
handler_fn_t handle_sign_payment_txn;

static handler_fn_t* lookupHandler(uint8_t ins) {
	switch (ins) {
	case INS_GET_VERSION:    return handle_get_version;
	case INS_GET_PUBLIC_KEY: return handle_get_public_key;
	case INS_SIGN_PAYMENT_TXN:   return handle_sign_payment_txn;
	default:                 return NULL;
	}
}

// These are the offsets of various parts of a request APDU packet. INS
// identifies the requested command (see above), and P1 and P2 are parameters
// to the command.
#define CLA          0xE0
#define OFFSET_CLA   0x00
#define OFFSET_INS   0x01
#define OFFSET_P1    0x02
#define OFFSET_P2    0x03
#define OFFSET_LC    0x04
#define OFFSET_CDATA 0x05

// This is the main loop that reads and writes APDUs. It receives request
// APDUs from the computer, looks up the corresponding command handler, and
// calls it on the APDU payload. Then it loops around and calls io_exchange
// again. The handler may set the 'flags' and 'tx' variables, which affect the
// subsequent io_exchange call. The handler may also throw an exception, which
// will be caught, converted to an error code, appended to the response APDU,
// and sent in the next io_exchange call.

static void helium_main(void) {

	STACK_CANARY = 0xDEADBEEF;

	volatile unsigned int rx = 0;
	volatile unsigned int tx = 0;
	volatile unsigned int flags = 0;

	// Exchange APDUs until EXCEPTION_IO_RESET is thrown.
	for (;;) {
		volatile unsigned short sw = 0;

		// The Ledger SDK implements a form of exception handling. In addition
		// to explicit THROWs in user code, syscalls (prefixed with os_ or
		// cx_) may also throw exceptions.
		//
		// In helium_main, this TRY block serves to catch any thrown exceptions
		// and convert them to response codes, which are then sent in APDUs.
		// However, EXCEPTION_IO_RESET will be re-thrown and caught by the
		// "true" main function defined at the bottom of this file.
		BEGIN_TRY {
			TRY {
				rx = tx;
				tx = 0; // ensure no race in CATCH_OTHER if io_exchange throws an error
				rx = io_exchange(CHANNEL_APDU | flags, rx);
				flags = 0;

				// No APDU received; trigger a reset.
				if (rx == 0) {
					THROW(EXCEPTION_IO_RESET);
				}
				// Malformed APDU.
				if (G_io_apdu_buffer[OFFSET_CLA] != CLA) {
					THROW(0x6E00);
				}
				// Lookup and call the requested command handler.
				handler_fn_t *handlerFn = lookupHandler(G_io_apdu_buffer[OFFSET_INS]);
				if (!handlerFn) {
					THROW(0x6D00);
				}
				handlerFn(G_io_apdu_buffer[OFFSET_P1], G_io_apdu_buffer[OFFSET_P2],
						G_io_apdu_buffer + OFFSET_CDATA, G_io_apdu_buffer[OFFSET_LC], &flags, &tx);

			}
			CATCH(EXCEPTION_IO_RESET) {
				THROW(EXCEPTION_IO_RESET);
			}
			CATCH_OTHER(e) {
				// Convert the exception to a response code. All error codes
				// start with 6, except for 0x9000, which is a special
				// "success" code. Every APDU payload should end with such a
				// code, even if no other data is sent. For example, when
				// calcTxnHash is processing packets of txn data, it replies
				// with just 0x9000 to indicate that it is ready to receive
				// more data.
				//
				// If the first byte is not a 6, mask it with 0x6800 to
				// convert it to a proper error code. I'm not totally sure why
				// this is done; perhaps to handle single-byte exception
				// codes?
				switch (e & 0xF000) {
				case 0x6000:
				case 0x9000:
					sw = e;
					break;
				default:
					sw = 0x6800 | (e & 0x7FF);
					break;
				}
				G_io_apdu_buffer[tx++] = sw >> 8;
				G_io_apdu_buffer[tx++] = sw & 0xFF;
			}
			FINALLY {
			}
		}
		END_TRY;
	}
}


// Everything below this point is Ledger magic. And the magic isn't well-
// documented, so if you want to understand it, you'll need to read the
// source, which you can find in the nanos-secure-sdk repo. Fortunately, you
// don't need to understand any of this in order to write an app.

// override point, but nothing more to do
void io_seproxyhal_display(const bagl_element_t *element) {
	io_seproxyhal_display_default((bagl_element_t *)element);
}

unsigned char G_io_seproxyhal_spi_buffer[IO_SEPROXYHAL_BUFFER_SIZE_B];

unsigned char io_event(unsigned char channel) {
	// can't have more than one tag in the reply, not supported yet.
	switch (G_io_seproxyhal_spi_buffer[0]) {
	case SEPROXYHAL_TAG_FINGER_EVENT:
		UX_FINGER_EVENT(G_io_seproxyhal_spi_buffer);
		break;

	case SEPROXYHAL_TAG_BUTTON_PUSH_EVENT:
		UX_BUTTON_PUSH_EVENT(G_io_seproxyhal_spi_buffer);
		break;

	case SEPROXYHAL_TAG_STATUS_EVENT:
		if (G_io_apdu_media == IO_APDU_MEDIA_USB_HID &&
			!(U4BE(G_io_seproxyhal_spi_buffer, 3) &
			  SEPROXYHAL_TAG_STATUS_EVENT_FLAG_USB_POWERED)) {
			THROW(EXCEPTION_IO_RESET);
		}
		UX_DEFAULT_EVENT();
		break;

	case SEPROXYHAL_TAG_DISPLAY_PROCESSED_EVENT:
		UX_DISPLAYED_EVENT({});
		break;

	case SEPROXYHAL_TAG_TICKER_EVENT:
		UX_TICKER_EVENT(G_io_seproxyhal_spi_buffer, {});
		break;

	default:
		UX_DEFAULT_EVENT();
		break;
	}

	// close the event if not done previously (by a display or whatever)
	if (!io_seproxyhal_spi_is_status_sent()) {
		io_seproxyhal_general_status();
	}

	// command has been processed, DO NOT reset the current APDU transport
	return 1;
}

unsigned short io_exchange_al(unsigned char channel, unsigned short tx_len) {
	switch (channel & ~(IO_FLAGS)) {
	case CHANNEL_KEYBOARD:
		break;
	// multiplexed io exchange over a SPI channel and TLV encapsulated protocol
	case CHANNEL_SPI:
		if (tx_len) {
			io_seproxyhal_spi_send(G_io_apdu_buffer, tx_len);
			if (channel & IO_RESET_AFTER_REPLIED) {
				reset();
			}
			return 0; // nothing received from the master so far (it's a tx transaction)
		} else {
			return io_seproxyhal_spi_recv(G_io_apdu_buffer, sizeof(G_io_apdu_buffer), 0);
		}
	default:
		THROW(INVALID_PARAMETER);
	}
	return 0;
}

static void app_exit(void) {
	BEGIN_TRY_L(exit) {
		TRY_L(exit) {
			os_sched_exit(-1);
		}
		FINALLY_L(exit) {
		}
	}
	END_TRY_L(exit);
}

__attribute__((section(".boot"))) int main(void) {
	// exit critical section
	__asm volatile("cpsie i");

	for (;;) {
		UX_INIT();
		os_boot();
		BEGIN_TRY {
			TRY {
				io_seproxyhal_init();
				USB_power(0);
				USB_power(1);
				ui_idle();
				helium_main();
			}
			CATCH(EXCEPTION_IO_RESET) {
				// reset IO and UX before continuing
				continue;
			}
			CATCH_ALL {
				break;
			}
			FINALLY {
			}
		}
		END_TRY;
	}
	app_exit();
	return 0;
}
