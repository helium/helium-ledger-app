#*******************************************************************************
#   Ledger App
#   (c) 2017 Ledger
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#*******************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif
include $(BOLOS_SDK)/Makefile.defines

ifeq ($(TESTNET),true)
DEFINES += HELIUM_TESTNET
endif

#########
#  App  #
#########
# The --path argument here restricts which BIP32 paths the app is allowed to derive.
ifeq ($(TESTNET),true)
APPNAME    = TNT
else
APPNAME    = Helium
endif


ifeq ($(TARGET_NAME),TARGET_NANOX)
	ICONNAME   = nanox_app_helium.gif
else
	ICONNAME   = nanos_app_helium.gif
endif

APPVERSION = 2.2.1

# The --path argument here restricts which BIP32 paths the app is allowed to derive.
ifeq ($(TESTNET),true)
APP_LOAD_PARAMS = --appFlags 0x240 --path "44'/905'" --curve secp256k1 --curve ed25519 $(COMMON_LOAD_PARAMS)
else
APP_LOAD_PARAMS = --appFlags 0x240 --path "44'/904'" --curve secp256k1 --curve ed25519 $(COMMON_LOAD_PARAMS)
endif

# Add security review banner. To be removed once Ledger security review is done.
APP_LOAD_PARAMS += --tlvraw 9F:01
DEFINES += HAVE_PENDING_REVIEW_SCREEN


APP_SOURCE_PATH = src
SDK_SOURCE_PATH = lib_stusb lib_stusb_impl

ifeq ($(TARGET_NAME),TARGET_NANOX)
	SDK_SOURCE_PATH  += lib_blewbxx lib_blewbxx_impl
	SDK_SOURCE_PATH  += lib_ux
endif

all: default

load: all
	python -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

delete:
	python -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

############
# Platform #
############


DEFINES += OS_IO_SEPROXYHAL
DEFINES += HAVE_BAGL HAVE_SPRINTF
DEFINES += HAVE_IO_USB HAVE_L4_USBLIB IO_USB_MAX_ENDPOINTS=7 IO_HID_EP_LENGTH=64 HAVE_USB_APDU
DEFINES += APPVERSION=\"$(APPVERSION)\"
DEFINES   +=  LEDGER_MAJOR_VERSION=$(APPVERSION_M) LEDGER_MINOR_VERSION=$(APPVERSION_N) LEDGER_PATCH_VERSION=$(APPVERSION_P)


ifeq ($(TARGET_NAME),TARGET_NANOX)
DEFINES       += IO_SEPROXYHAL_BUFFER_SIZE_B=300
DEFINES       += HAVE_BLE BLE_COMMAND_TIMEOUT_MS=2000
DEFINES       += HAVE_BLE_APDU # basic ledger apdu transport over BLE

DEFINES       += HAVE_BAGL BAGL_WIDTH=128 BAGL_HEIGHT=64
DEFINES       += HAVE_BAGL_FONT_OPEN_SANS_REGULAR_11PX
DEFINES       += HAVE_BAGL_FONT_OPEN_SANS_EXTRABOLD_11PX
DEFINES       += HAVE_BAGL_FONT_OPEN_SANS_LIGHT_16PX
DEFINES       += HAVE_UX_FLOW
else
	DEFINES       += IO_SEPROXYHAL_BUFFER_SIZE_B=128
	DEFINES	      += HAVE_UX_LEGACY
endif

# Enabling debug PRINTF
DEBUG = 0
ifdef DEBUG
	ifeq ($(TARGET_NAME),TARGET_NANOX)
		DEFINES   += HAVE_PRINTF PRINTF=mcu_usb_printf
	else
		DEFINES   += HAVE_PRINTF PRINTF=screen_printf
	endif
else
	DEFINES   += PRINTF\(...\)=
endif


##############
#  Compiler  #
##############

CC := $(CLANGPATH)clang
CFLAGS += -O3 -Os

AS := $(GCCPATH)arm-none-eabi-gcc
LD := $(GCCPATH)arm-none-eabi-gcc
LDFLAGS += -O3 -Os 
LDLIBS += -lm -lgcc -lc

##################
#  Dependencies  #
##################

INCLUDES_PATH += ./

# import rules to compile glyphs
include $(BOLOS_SDK)/Makefile.glyphs
# import generic rules from the sdk
include $(BOLOS_SDK)/Makefile.rules

dep/tx.d:

dep/%.d: %.c Makefile

listvariants:
	@echo VARIANTS COIN hnt
