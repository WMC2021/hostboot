# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: makefile $
#
# OpenPOWER HostBoot Project
#
# Contributors Listed Below - COPYRIGHT 2010,2019
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG

SUBDIRS = src.d
ROOTPATH = .

CONFIG_FILE ?= default

GEN_PASS_PRE += $(GENDIR)/.$(notdir $(CONFIG_FILE)).config
CLEAN_TARGETS += $(wildcard $(GENDIR)/.*.config)
CLEAN_TARGETS += $(GENDIR)/config.mk $(GENDIR)/config.h
SKIP_CONFIG_FILE_LOAD = 1

IMAGE_PASS_POST += $(GENDIR)/hwp_id.html
CLEAN_TARGETS   += $(GENDIR)/hwp_id.html

# Name of file to indicate which release Hostboot is building for
HB_FSP_RELEASE = $(GENDIR)/hb_fsp_release
HB_P10_RELEASE = $(GENDIR)/hb_simics_p10_release

ifndef BUILD_MINIMAL
IMAGE_PASS_POST += cscope ctags
endif
IMAGE_PASS_POST += check_istep_modules

include ./config.mk

.PHONY: docs
docs: src/build/doxygen/doxygen.conf
	rm -rf obj/doxygen/*
	doxygen src/build/doxygen/doxygen.conf

.PHONY: citest
citest:
	src/build/citest/cxxtest-start.sh

.PHONY: gcov
gcov:
	rm -rf obj/gcov/*
	$(MAKE) gcov_pass
	find obj/gcov/ -size 0c | xargs rm # Delete empty files.
	genhtml obj/gcov/*.lcov -o obj/gcov/html --prefix `pwd` \
	    --title `git describe --dirty`
	@echo "View GCOV results with: firefox obj/gcov/html/index.html"

$(GENDIR)/hwp_id.html :
	$(ROOTPATH)/src/build/tools/hwp_id.pl -i -l > $@

.PHONY: check_istep_modules
check_istep_modules: $(OBJS)
	listdeps.pl $(IMGDIR)  -v

GENCONFIG_TOOL = src/build/tools/hbGenConfig

# At end of rule, create HB_FSP_RELEASE file if compiling with fsprelease.config
# or create HB_P10_RELEASE if compiling with simics_p10.config
$(GENDIR)/.$(notdir $(CONFIG_FILE)).config: \
    $(shell find -name HBconfig) \
    $(filter-out $(GENDIR)/.$(notdir $(CONFIG_FILE)).config,\
	    $(wildcard $(GENDIR)/.*.config)) \
    $(GENCONFIG_TOOL) \
    $(filter-out default,$(CONFIG_FILE))
	@mkdir -p $(GENDIR)
	$(C2) "    GENCONFIG"
	$(C1)$(GENCONFIG_TOOL) $(CONFIG_FILE) \
	    $(filter-out $(GENCONFIG_TOOL) $(CONFIG_FILE) \
			 $(wildcard $(GENDIR)/.*.config),$^)
	@rm -f $(wildcard $(GENDIR)/.*.config)
	@touch $@
	@rm -f $(HB_FSP_RELEASE)
	@rm -f $(HB_P10_RELEASE)
    ifneq (,$(findstring fsprelease.config, $(strip $(CONFIG_FILE))))
	    @touch $(HB_FSP_RELEASE)
    endif
    ifneq (,$(findstring simics_p10.config, $(strip $(CONFIG_FILE))))
	    @touch $(HB_P10_RELEASE)
    endif
