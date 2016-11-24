Name: telegram-qt
Summary: Telegram bindings for Qt
%define version_major 0
%define version_minor 2
%define version_patch 0

Version: %{version_major}.%{version_minor}.%{version_patch}
Release: 1
Group: System/Libraries
License: LGPLv2.1
URL: https://github.com/Kaffeine/telegram-qt
Source0: https://github.com/Kaffeine/telegram-qt/releases/download/telegram-qt-%{version}/telegram-qt-%{version}.tar.bz2
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Network)
BuildRequires: pkgconfig(openssl)

%description
%{summary}.

%package qt5
Summary: TelegramQt library for Qt5
Group: Development/Libraries
Requires: qt5-qtcore
Requires: qt5-qtnetwork

%description qt5
%{summary}.

%package qt5-devel
Summary:    Development headers and pkg-config for TelegramQt library
Group:      Development/Libraries
Requires:   %{name}-qt5%{?_isa} = %{version}-%{release}
%description qt5-devel
%{summary}.

# Fallback to qtc_qmake5 to qmake5, if the first one is not already defined
%{!?qtc_qmake5:%define qtc_qmake5 %qmake5}

%prep
%setup -q -n %{name}-%{version}

%build
%{qtc_qmake5} \
  "BUILD_ONLY_LIBRARY=true" \
  "INSTALL_PREFIX=%{_prefix}" \
  "INSTALL_LIB_DIR=%{_libdir}" \
  "INSTALL_INCLUDE_DIR=%{_includedir}"

make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files qt5
%defattr(-,root,root,-)
%{_libdir}/libTelegramQt5.so
%{_libdir}/libTelegramQt5.so.%{version_major}
%{_libdir}/libTelegramQt5.so.%{version_major}.%{version_minor}
%{_libdir}/libTelegramQt5.so.%{version_major}.%{version_minor}.%{version_patch}

%files qt5-devel
%defattr(-,root,root,-)
%{_includedir}/telegram-qt5/TelegramQt/*
%{_libdir}/pkgconfig/TelegramQt5.pc
