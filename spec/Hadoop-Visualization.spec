Name:           Hadoop-Visualization
Version:        0.0.2
Release:        1%{?dist}
Summary:        A program that visualized the data movement through a Hadoop DFS pool.

Group:          Amusements/Graphics
License:        GPL
URL:            http://derek.unl.edu/hadoopviz/%{name}-%{version}-%{release}.tar.gz
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  SDL-devel
Requires:       SDL

%description


%prep
%setup -q


%build
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT BINDIR=%{_bindir} DATADIR=%{_datadir} LIBEXECDIR=%{_libexecdir} install


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
%{_bindir}/hadoopviz
%{_libexecdir}/hadoopviz
%{_datadir}/hadoopviz/*

%changelog


