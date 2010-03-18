Name:           hadoop-visualization
Version:        0.0.1
Release:        3%{?dist}
Summary:        A program that visualized the data movement through a Hadoop DFS pool.

Group:          Amusements/Graphics
License:        GPL
URL:            http://derek.unl.edu/hadoopviz/%{name}-%{version}.tar.gz
Source0:        %{name}-%{version}.tgz
Patch0:		%{name}-0.0.1-gl.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  SDL-devel freetype-devel libGL-devel libGLU-devel
Requires:       SDL gratia-probe-common

%description


%prep
%setup -q
%patch0 -p0


%build
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT BINDIR=%{_bindir} DATADIR=%{_datadir} LIBEXECDIR=%{_libexecdir} install
install -p Server/syslogserv.py $RPM_BUILD_ROOT%{_bindir}/hadoopviz-server
install -p Server/syslogserv-gratia.py $RPM_BUILD_ROOT%{_bindir}/hadoopviz-server-gratia


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
%{_bindir}/hadoopviz
%{_bindir}/hadoopviz-server
%{_bindir}/hadoopviz-server-gratia
%{_libexecdir}/*
%{_datadir}/*

%changelog

