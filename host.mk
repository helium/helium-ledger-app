ifeq ($(BOLOS_SDK),)
$(error BOLOS_SDK is not set)
endif

BOLOS_ENV = bogus

ifeq ($(TARGET_NAME),TARGET_NANOX)
	ICONNAME=icons/nanox_app_solana.gif
else
	ICONNAME=icons/nanos_app_solana.gif
endif

include config.min
include $(BOLOS_SDK)/Makefile.defines

.PHONY: load
load:
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

.PHONY: load-offline
load-offline:
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS) --offline

.PHONY: delete
delete:
	python3 -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

.PHONY: release
release: install.sh

install.sh: host.mk requirements.txt bin/app.hex
	echo > install.sh

	export APP_CODE="$$(cat bin/app.hex)"; \
	export APP_REQUIREMENTS="$$(cat requirements.txt)"; \
	export APP_LOAD_PARAMS_EVALUATED="$(shell printf '\\"%s\\" ' $(APP_LOAD_PARAMS:bin/%=%))"; \
	cat install-template.sh | envsubst '$$APP_LOAD_PARAMS_EVALUATED $$APP_CODE $$APP_REQUIREMENTS' >> install.sh

	chmod +x install.sh

.PHONY: deps
deps:
	python3 -mpip install -r requirements.txt --require-hashes
