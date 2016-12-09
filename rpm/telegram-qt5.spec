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
BuildRequires: cmake >= 2.8

%description
%{summary}.

%package qt5
Summary: TelegramQt library for Qt5
Group: Development/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires: qt5-qtcore
Requires: qt5-qtnetwork
Requires: openssl

%description qt5
%{summary}.

%package qt5-devel
Summary:    Development headers and pkg-config for TelegramQt library
Group:      Development/Libraries
Requires:   %{name}-qt5%{?_isa} = %{version}-%{release}
%description qt5-devel
%{summary}.

%prep
%setup -q

%build
%cmake -DENABLE_TESTAPP=FALSE

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
