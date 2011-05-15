
star_auto_sh=$(wildcard */star_auto.0.sh) $(wildcard */star_auto.1.sh) $(wildcard */star_auto.2.sh) $(wildcard */star_auto.lib.sh) $(wildcard */star_auto.3.sh) $(wildcard */star_auto.4.sh) $(wildcard */star_auto.5.sh) $(wildcard */star_auto.sh) $(wildcard */star_auto.6.sh) $(wildcard */star_auto.7.sh) $(wildcard */star_auto.8.sh) $(wildcard */star_auto.9.sh) 

star_auto_prefix=star_auto.makefile
star_auto_postfix=
star_auto_makefile=$(wildcard */$(star_auto_prefix).0$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).1$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).2$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).lib$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).3$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).4$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).hi$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).5$(star_auto_postfix)) $(wildcard */$(star_auto_prefix)$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).6$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).lo$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).7$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).8$(star_auto_postfix)) $(wildcard */$(star_auto_prefix).9$(star_auto_postfix)) 
PARAM= TARGET_ROOT=$(TARGET_ROOT) KERNELDIR=$(KERNELDIR) INSTALL_PATH=$(TARGET_ROOT) CC=${CC}


star_auto_install: chk_param
	@echo -------------------------------------------------------------------------------
	@echo $@
	@echo star_auto_sh=${star_auto_sh}
	@echo -------------------------------------------------------------------------------
	@cd ${APP_DIR};func=install;for x in ${star_auto_sh} ; do echo $$x $$func;$$x $$func $(TARGET_ROOT) $(KERNELDIR)||exit 1; done
	@cd ${APP_DIR};func=install;for x in ${star_auto_makefile} ; do echo $$x $$func;$(MAKE) -f `basename $$x` -C `dirname $$x` $$func ${PARAM} || exit 1;done

star_auto_build: chk_param
	@echo -------------------------------------------------------------------------------
	@echo $@
	@echo star_auto_sh=${star_auto_sh}
	@echo -------------------------------------------------------------------------------
	@cd ${APP_DIR};func=build;for x in ${star_auto_sh} ; do echo $$x $$func;$$x $$func $(TARGET_ROOT) $(KERNELDIR)||exit 1; done
	@cd ${APP_DIR};func=build;for x in ${star_auto_makefile} ; do echo $$x $$func;$(MAKE) -f `basename $$x` -C `dirname $$x` $$func ${PARAM} || exit 1;done

star_auto_clean: chk_param
	@echo -------------------------------------------------------------------------------
	@echo $@
	@echo star_auto_sh=${star_auto_sh}
	@echo -------------------------------------------------------------------------------
	@cd ${APP_DIR};func=clean;for x in ${star_auto_sh} ; do echo $$x $$func;$$x $$func $(TARGET_ROOT) $(KERNELDIR)||exit 1; done
	@cd ${APP_DIR};func=clean;for x in ${star_auto_makefile} ; do echo $$x $$func;$(MAKE) -f `basename $$x` -C `dirname $$x` $$func ${PARAM} || exit 1;done

chk_param:
	@if [ "$(TARGET_ROOT)x" = "x" ] ;then echo TARGET_ROOT not set!!;exit 1;fi;
	@if [ "$(KERNELDIR)x" = "x" ] ;then echo KERNELDIR not set!!;exit 1;fi;
