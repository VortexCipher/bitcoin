define int_vars
#Set defaults for vars which may be overridden per-package
$(1)_cc=$$($$($(1)_type)_CC)
$(1)_cxx=$$($$($(1)_type)_CXX)
$(1)_objc=$$($$($(1)_type)_OBJC)
$(1)_objcxx=$$($$($(1)_type)_OBJCXX)
$(1)_ar=$$($$($(1)_type)_AR)
$(1)_ranlib=$$($$($(1)_type)_RANLIB)
$(1)_nm=$$($$($(1)_type)_NM)
$(1)_cflags=$$($$($(1)_type)_CFLAGS) \
            $$($$($(1)_type)_$$(release_type)_CFLAGS)
$(1)_cxxflags=$$($$($(1)_type)_CXXFLAGS) \
              $$($$($(1)_type)_$$(release_type)_CXXFLAGS)
$(1)_ldflags=$$($$($(1)_type)_LDFLAGS) \
             $$($$($(1)_type)_$$(release_type)_LDFLAGS) \
             -L$$($($(1)_type)_prefix)/lib
$(1)_cppflags=$$($$($(1)_type)_CPPFLAGS) \
              $$($$($(1)_type)_$$(release_type)_CPPFLAGS) \
              -I$$($$($(1)_type)_prefix)/include
$(1)_recipe_hash:=
endef

define int_get_all_dependencies
$(sort $(foreach dep,$(2),$(2) $(call int_get_all_dependencies,$(1),$($(dep)_dependencies))))
endef

define fetch_file_inner
    ( mkdir -p $$($(1)_download_dir) && echo Fetching $(3) from $(2) && \
    $(build_DOWNLOAD) "$$($(1)_download_dir)/$(4).temp" "$(2)/$(3)" && \
