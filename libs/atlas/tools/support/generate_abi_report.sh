#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Performs an ABI check against an older version, using the ABI_compliance_checker tool."
  echo "You must have the ABI_compliance_checker installed to use this script. Get it from http://ispras.linuxbase.org/index.php/ABI_compliance_checker"
  echo "This script must be run from the Git source directory."
  echo "Usage: `basename $0` <older_version>"
  echo "<older_version> is the name of an older tag (or commit) which this script will check out from Git"
  exit -1
fi

command -v git >/dev/null 2>&1 || { echo "Git command not available." > $1; exit 0; }
command -v abi-compliance-checker >/dev/null 2>&1 || { echo "abi-compliance-checker command not available; get it from http://ispras.linuxbase.org/index.php/ABI_compliance_checker." > $1; exit 0; }

SOURCE_DIR=$PWD

#Figure out the name of the lib by looking in "configure"
LIBRARY_NAME=`grep PACKAGE_NAME= $SOURCE_DIR/configure | sed -e s/PACKAGE_NAME=\'//g | sed -e s/\'//g`

ABI_REPORT_DIR=$PWD/abi_report

#Disable warnings to counteract -Werror (older code might cause warnings with newer compilers, and fail through -Werror).
export CXXFLAGS="$CXXFLAGS -w"

OLD_VERSION=$1
OLD_SOURCE_DIR=$ABI_REPORT_DIR/${OLD_VERSION}_source
OLD_BUILD_DIR=$ABI_REPORT_DIR/${OLD_VERSION}_build
OLD_INSTALL_DIR=$ABI_REPORT_DIR/${OLD_VERSION}_install
MASTER_BUILD_DIR=$ABI_REPORT_DIR/master_build
MASTER_INSTALL_DIR=$ABI_REPORT_DIR/master_install

mkdir -p $ABI_REPORT_DIR
mkdir -p $OLD_SOURCE_DIR
mkdir -p $OLD_BUILD_DIR
mkdir -p $MASTER_BUILD_DIR

#Check out the older version into a separate tree
git archive $OLD_VERSION | tar -x -C $OLD_SOURCE_DIR

if [ $? != 0 ]
then
  echo "Could not check out '$OLD_VERSION'. Make sure that this is something that's available in Git."
  exit -1
fi

echo "Building master version"
cd $MASTER_BUILD_DIR && $SOURCE_DIR/configure --prefix $MASTER_INSTALL_DIR && make install

echo "Building older version"
cd $OLD_SOURCE_DIR && NOCONFIGURE=1 ./autogen.sh && cd $OLD_BUILD_DIR && $OLD_SOURCE_DIR/configure --prefix $OLD_INSTALL_DIR && make install

if [ $? != 0 ]
then
  echo "Error when building old version."
  exit -1
fi

cd $ABI_REPORT_DIR
echo "Performing ABI compliance check"
abi-compliance-checker -l $LIBRARY_NAME -v1 $OLD_VERSION -v2 master -old $OLD_INSTALL_DIR -new $MASTER_INSTALL_DIR


