Testing environment (CTS)  {#auto_testing}
=========================

This new test environment from Tizen is the Conformance Test Suite (CTS) which will replace Dali TCs to UTCs in Tizen TCT in the near future.

These are the set of commands related to build and installation of Automated Test Cases.

To build for target, first build and install dali-core, dali-adaptor and dali-toolkit, then build dali-capi without --keep-packs option.

You will need to install libconfig-tiny-perl:

sudo apt-get install libconfig-tiny-perl

If you use a non-standard `GBS_ROOT` then you will need to edit the tcbuild script to match your configuration - change lines 98 and 108 and add a -B option with your GBS-ROOT path ( `gbs build -A armv7l --spec ... ` ).
To install on device from a non-standard GBS_ROOT, also modify line 28 (`RPM_DIR="$HOME/GBS-ROOT/local/repos/$PROFILE/armv7l/RPMS"`).


1. Building the tests on target device
======================================

For core Dali cd into automated-tests, and use:

  (a) Building Cts 
  -------------------
    ./tcbuild build Cts dali

  (b) Building Utc
  -------------------
    ./tcbuild build utc dali


2. Installing the tests on target device 
========================================

Install process is still under process.
Readme shall be updated shortly.
