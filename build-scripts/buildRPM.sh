#!/bin/bash

tmpDir=`mktemp -d`

cd $tmpDir

svn co svn://t2.unl.edu/brian/HadoopViz/trunk Hadoop-Visualization-$1

tar czf Hadoop-Visualization-$1.tar.gz Hadoop-Visualization-$1

cp -f Hadoop-Visualization-$1.tar.gz ~/rpmbuild/SOURCES

rm -rf $tmpDir
