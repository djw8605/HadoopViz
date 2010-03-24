Name:           hadoop-visualization
Version:        0.0.1
Release:        6%{?dist}
Summary:        A program that visualized the data movement through a Hadoop DFS pool.

Group:          Amusements/Graphics
License:        GPL
URL:            http://derek.unl.edu/hadoopviz/%{name}-%{version}.tar.gz
# Create source 0 with:
# svn co svn://t2.unl.edu/brian/HadoopViz/trunk hadoop-visualization-0.0.1
# tar czf hadoop-visualization-0.0.1.tgz hadoop-visualization-0.0.1
Source0:        %{name}-%{version}.tgz
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

%prep
%setup -q

%build
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT BINDIR=%{_bindir} DATADIR=%{_datadir} LIBEXECDIR=%{_libexecdir} install
install -p Server/syslogserv.py $RPM_BUILD_ROOT%{_bindir}/hadoopviz-server
mkdir -p $RPM_BUILD_ROOT%{_initrddir}
install -m 0755 Server/hadoopviz.init $RPM_BUILD_ROOT%{_initrddir}/hadoopviz
mkdir -p $RPM_BUILD_ROOT%{_var}/run/hadoopviz


%clean
rm -rf $RPM_BUILD_ROOT

%pre server
getent group hadoopviz >/dev/null || groupadd -r hadoopviz
getent passwd hadoopviz >/dev/null || \
       useradd -r -g hadoopviz -c "hadoopviz runtime user" \
       -s /sbin/nologin -d /etc/hadoopviz hadoopviz
exit 0

%post server
/sbin/chkconfig --add hadoopviz

%preun server
if [ "$1" = "0" ]; then
    /sbin/service hadoopviz stop > /dev/null 2>&1
    /sbin/chkconfig --del hadoopviz
fi

%postun server
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
%attr(-,hadoopviz,hadoopviz) %dir %{_var}/run/hadoopviz

%changelog
* Wed Mar 24 2010 Michael Thomas <thomas@hep.caltech.edu> 0.0.1-6
- Improvements to damonization/pid file handling

* Mon Mar 22 2010 Michael Thomas <thomas@hep.caltech.edu> 0.0.1-5
- Add patch for converting IP addresses to hostnames
- Add patch for forking the server into the background
- Add init script for starting the server

* Wed Mar 17 2010 Michael Thomas <thomas@hep.caltech.edu> 0.0.1-4
- Split the client and server components into separate packages

