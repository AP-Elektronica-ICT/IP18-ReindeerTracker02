import { Component } from '@angular/core';
import {AlertController, NavController} from 'ionic-angular';
import { DetailPage } from '../detail/detail';
import { DeletePage } from '../delete/delete';
import { AddPage } from '../add/add';
import { ProfilePage } from '../profile/profile';
import { LogInPage } from '../log-in/log-in';
import {DeviceProvider} from "../../providers/device/device";
import {BarcodeScanner} from "@ionic-native/barcode-scanner";

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {
  devices: any = null;
  message = '';

  constructor(public navCtrl: NavController, private deviceProvider: DeviceProvider, private alert: AlertController, public barcodeScanner: BarcodeScanner) {

  }

  ionViewWillEnter() {
    this.getDevices();
  }

  getDevices() {
    this.deviceProvider.getUserDevices()
      .subscribe(res => {
        this.devices = res;
        console.log(this.devices);
      })
  }

  showKeyInputSelectAlert() {
    let confirm = this.alert.create({
      title: 'Select input',
      message: 'The device key can be added manualy or can be scanned via the qr code',
      buttons: [
        {
          text: 'Manual',
          handler: () => {
            this.showManualKeyInputAlert();
          }
        },
        {
          text: 'Scan QR',
          handler: () => {
            this.scanQr();
          }
        }
      ]
    });
    confirm.present();
  }

  showManualKeyInputAlert() {
    let prompt = this.alert.create({
      title: 'Device key',
      message: "Please enter the key that is displayed on the device",
      inputs: [
        {
          name: 'key',
          placeholder: 'Ex: 123456',
          type: 'number'
        },
      ],
      buttons: [
        {
          text: 'Cancel',
          handler: data => {
            console.log('Cancel clicked');
          }
        },
        {
          text: 'Next',
          handler: data => {
            const deviceKey = data.key;
            this.deviceProvider.addDeviceToUser(deviceKey)
              .subscribe(res => {
                this.navCtrl.push(AddPage, {deviceKey: deviceKey});
              }, err => {
                this.showDeviceErrorAlert();
              });
          }
        }
      ]
    });
    prompt.present();
  }

  showDeviceErrorAlert() {
    let alert = this.alert.create({
      title: 'Device not found',
      subTitle: 'The device you tried to add could not be found.',
      buttons: ['OK']
    });
    alert.present();
  }

  scanQr(){
    this.barcodeScanner.scan().then(barcodeData => {
      const deviceKey = barcodeData.text;
      this.deviceProvider.addDeviceToUser(deviceKey)
        .subscribe(res => {
          this.navCtrl.push(AddPage, {deviceKey: deviceKey});
        }, err => {
          this.showDeviceErrorAlert();
        });
    }).catch(err => {
      this.message = err;
    })
  }

  loadDetail(deviceKey: string){
    this.navCtrl.push(DetailPage, deviceKey);
  }

  loadDelete(){
    this.navCtrl.push(DeletePage);
  }

  loadAdd(){
    this.showKeyInputSelectAlert();
  }

  loadProfile(){
    this.navCtrl.push(ProfilePage);
  }

  loadLogOut(){
    this.navCtrl.push(LogInPage);
  }

}
