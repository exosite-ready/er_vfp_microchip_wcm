#!/bin/bash

CLIENT_DIR=`pwd`
PLATFORM_DIR=$CLIENT_DIR/platforms/er_plat_microchip_wcm
cd ../3rd_party
patch -Np1 < "$PLATFORM_DIR/WCM_DK1.patch"