Name:       dali
Summary:    The OpenGLES Canvas Core Library
Version:    0.9.5
Release:    1
Group:      System/Libraries
License:    Flora
URL:        TO_BE_FILLED
Source0:    %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:       boost-thread
BuildRequires:  pkgconfig
BuildRequires:  opengl-es-devel
BuildRequires:  gawk
BuildRequires:  boost-devel
BuildRequires:  boost-thread
BuildRequires:  sec-product-features

%description
The OpenGLES Canvas Core Library

##############################
# devel
##############################
# This should be used only for building dali-adaptor and dali-toolkit independently of
# dali-capi - i.e. to break cyclic dependencies.
%package devel
Summary:    Development components for the OpenGLES Canvas Core Library
Group:      Development/Libs
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-integration-devel = %{version}-%{release}
%ifarch %{ix86}
Requires:   opengl-es-devel
%endif

%description devel
Development components for the OpenGLES Canvas Core Library

##############################
# integration-devel
##############################
%package integration-devel
Summary:    Integration development package for the OpenGLES Canvas
Group:      Development/Libs
Requires:   %{name} = %{version}-%{release}

%description integration-devel
Integration development package for the OpenGLES Canvas

##############################
# test-suite-utils
##############################
%package test-suite-utils
Summary:    Automated Test Suite Framework for OpenGLES Canvas Library
Group:      Development/Libs
Requires:   %{name} = %{version}-%{release}

%description test-suite-utils
Automated Test Suite Framework for OpenGLES Canvas Library

##############################
# Preparation
##############################
%prep
%setup -q
%define dali_data_rw_dir /opt/usr/share/dali/
%define dali_data_ro_dir /usr/share/dali/
%define shader_bin_dir   %{dali_data_rw_dir}/core/shaderbin
%define dev_include_path %{_includedir}

##############################
# Build
##############################
%build
PREFIX="/usr"
CXXFLAGS+=" -Wall -g -O2 -DNDEBUG -fPIC -fvisibility-inlines-hidden -fdata-sections -ffunction-sections "
LDFLAGS+=" -Wl,--rpath=$PREFIX/lib -Wl,--as-needed -Wl,--gc-sections -lgcc_s -lgcc -lpthread"

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%ifarch %{arm}
CXXFLAGS+=" -D_ARCH_ARM_ -mfpu=neon"
%endif

libtoolize --force
cd %{_builddir}/%{name}-%{version}/build/slp && autoreconf --install
cd %{_builddir}/%{name}-%{version}/build/slp && CXXFLAGS="$CXXFLAGS" LDFLAGS="$LDFLAGS" DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}" ./configure --prefix="$PREFIX"

make %{?jobs:-j%jobs}

##############################
# Installation
##############################
%install
rm -rf %{buildroot}
cd build/slp
%make_install DALI_DATA_RW_DIR="%{dali_data_rw_dir}" DALI_DATA_RO_DIR="%{dali_data_ro_dir}"

# LICENSE
mkdir -p %{buildroot}/usr/share/license
cp -af %{_builddir}/%{name}-%{version}/packaging/LICENSE %{buildroot}/usr/share/license/%{name}

%if 0%{?sec_product_feature_graphics_adreno}
  mkdir -p %{buildroot}%{shader_bin_dir}
  mv %{_builddir}/%{name}-%{version}/dali/shaderbins/adreno330/*.dali-bin %{buildroot}%{shader_bin_dir}
%endif

##############################
# Post Install
##############################
%post
/sbin/ldconfig
chown 5000:5000 %{shader_bin_dir}
%if 0%{?sec_product_feature_graphics_adreno}
%else
  rm -rf %{shader_bin_dir}/*
%endif
exit 0

##############################
# Post Uninstall
##############################
%postun
/sbin/ldconfig
rm -rf %{shader_bin_dir}
exit 0

##############################
# Files in Binary Packages
##############################

%files
%manifest dali.manifest
%defattr(-,root,root,-)
%{_libdir}/lib%{name}-core.so*
%defattr(-,app,app,-)
%if 0%{?sec_product_feature_graphics_adreno}
  %{shader_bin_dir}/*
%endif
%dir %{shader_bin_dir}
%{_datadir}/license/%{name}

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/*.pc
%{dev_include_path}/%{name}/public-api/*

%files integration-devel
%defattr(-,root,root,-)
%{_includedir}/%{name}/integration-api/*

%files test-suite-utils
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/%{name}-test-suite-utils.pc
%{_includedir}/%{name}/test-suite-utils/*
