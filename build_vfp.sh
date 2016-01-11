cd projects/EmbeddedClient-Microchip-WCM.X
PROJECT_PATH=`pwd`
/Applications/microchip/mplabx/v3.00/mplab_ide.app/Contents/Resources/mplab_ide/bin/prjMakefilesGenerator.sh $PROJECT_PATH
make
cp dist/default/production/EmbeddedClient-Microchip-WCM.X.production.hex ../../gw_wcm.hex
