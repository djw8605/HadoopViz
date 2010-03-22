Name:           hadoop-visualization
Version:        0.0.1
Release:        5%{?dist}
Summary:        A program that visualized the data movement through a Hadoop DFS pool.

Group:          Amusements/Graphics
License:        GPL
URL:            http://derek.unl.edu/hadoopviz/%{name}-%{version}.tar.gz
# Create source 0 with:
# svn co svn://t2.unl.edu/brian/HadoopViz/trunk hadoop-visualization-0.0.1
# tar czf hadoop-visualization-0.0.1.tgz hadoop-visualization-0.0.1
Source0:        %{name}-%{version}.tgz
Source1:        hadoopviz.init
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  SDL-devel freetype-devel libGL-devel libGLU-devel
Requires:       SDL

%description
3D tool for visualizing the data movement in a HDFS cluster

%package server
Summary: Server module for relaying HDFS event data
Group:   System Environment/Daemons
%description server
Server module for relaying HDFS event data

%package gratiaserver
Summary: Server module for relaying HDFS event data with gratia features
Group:   System Environment/Daemons
Requires:  gratia-probe-common
%description gratiaserver
Server module for relaying HDFS event data, with a gratia component that
relays HDFS usage data to a gratia server for long-term data collection.

%prep
%setup -q

%build
rm -rf $RPM_BUILD_ROOT
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT BINDIR=%{_bindir} DATADIR=%{_datadir} LIBEXECDIR=%{_libexecdir} install
install -p Server/syslogserv.py $RPM_BUILD_ROOT%{_bindir}/hadoopviz-server
install -p Server/syslogserv-gratia.py $RPM_BUILD_ROOT%{_bindir}/hadoopviz-gratiaserver
mkdir -p $RPM_BUILD_ROOT%{_initrddir}
install -m 0755 %{SOURCE1} $RPM_BUILD_ROOT%{_initrddir}/hadoopviz


%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/chkconfig --add hadoopviz

%preun
if [ "$1" = "0" ]; then
    /sbin/service hadoopviz stop > /dev/null 2>&1
    /sbin/chkconfig --del hadoopviz
fi

%postun
if [ "$1" -ge "1" ] ; then
    /sbin/service hadoopviz condrestart >/dev/null 2>&1 || :
fi

%files
%defattr(-,root,root,-)
%{_bindir}/hadoopviz
%{_libexecdir}/*
%{_datadir}/*

%files server
%defattr(-,root,root,-)
%{_bindir}/hadoopviz-server
%{_initrddir}/hadoopviz

%files gratiaserver
%defattr(-,root,root,-)
%{_bindir}/hadoopviz-gratiaserver

%changelog
* Mon Mar 22 2010 Michael Thomas <thomas@hep.caltech.edu> 0.0.1-5
- Add init script for starting the server

* Wed Mar 17 2010 Michael Thomas <thomas@hep.caltech.edu> 0.0.1-4
- Split the client and server components into separate packages

