BOLOS_SDK = nanos-secure-sdk
BOLOS_ENV = bogus
include $(BOLOS_SDK)/Makefile.defines
include config.min

ifeq ($(TARGET_NAME),TARGET_NANOX)
	ICONNAME=icons/nanox_app_solana.gif
else
	ICONNAME=icons/nanos_app_solana.gif
endif

load:
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS)

load-offline:
	python3 -m ledgerblue.loadApp $(APP_LOAD_PARAMS) --offline

delete:
	python3 -m ledgerblue.deleteApp $(COMMON_DELETE_PARAMS)

release:
	export APP_LOAD_PARAMS_EVALUATED="$(shell printf '\\"%s\\" ' $(APP_LOAD_PARAMS))"; \
	cat load-template.sh | envsubst > load.sh
	chmod +x load.sh
	tar -zcf solana-ledger-app-$(APPVERSION).tar.gz load.sh bin/app.hex
	rm load.sh

deps:
	python3 -mpip install ledgerblue
