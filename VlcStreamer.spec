Name:	vlcstreamer	
Version:	0.0.1
Release:	1%{?dist}
Summary:	A vlc streamer server for the iPhone VlcStreamerClient app

Group: Applications/Multimedia		
License:	GPL
URL:	https://github.com/mcgouganp/VLCStreamer	
Source0:	vlcstreamer-%{version}.tar.gz

BuildRequires:	
Requires:	

%description


%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%files
%doc



%changelog

