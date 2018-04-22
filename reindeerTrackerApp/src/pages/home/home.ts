import { Component } from '@angular/core';
import {AlertController, ModalController, NavController, Refresher} from 'ionic-angular';
import { DetailPage } from '../detail/detail';
import { DeletePage } from '../delete/delete';
import { AddPage } from '../add/add';
import { ProfilePage } from '../profile/profile';
import { LogInPage } from '../log-in/log-in';
import {DeviceProvider} from "../../providers/device/device";
import {BarcodeScanner} from "@ionic-native/barcode-scanner";
import {AuthProvider} from "../../providers/auth/auth";
import {LoadingProvider} from "../../providers/loading/loading";
import {AliveState, BatteryState, FilterOptions} from "../../classes/filterOptions";
import {Device} from "../../classes/device";
import {FilterPage} from "../filter/filter";
import {FilterProvider} from "../../providers/filter/filter";
import {FcmProvider} from "../../providers/fcm/fcm";
import {NotificationPage} from "../notification/notification";

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {
  displayingDevices: Device[] = [];
  devices: Device[] = [];
  message = '';
  filterOptions: FilterOptions = null;
  filterStrings: String[] = [];

  constructor(public navCtrl: NavController, private deviceProvider: DeviceProvider, private alert: AlertController, public barcodeScanner: BarcodeScanner, private auth: AuthProvider, private loading: LoadingProvider, private filterProvider: FilterProvider, private fcm: FcmProvider) {

  }

  ionViewDidLoad() {

  }

  ionViewWillEnter() {
    this.getDevices();
  }

  onRefresh(refresher: Refresher) {
    this.deviceProvider.loadUserDevices()
      .then(devices => {
        console.log(devices);
        this.afterDevicesLoaded(devices);
        refresher.complete();
      })
  }

  getDevices() {
    this.deviceProvider.getUserDevicesFromStorage()
      .then(devices => {
        this.afterDevicesLoaded(devices);
      })
  }

  afterDevicesLoaded(devices: Device[]) {
    this.devices = devices;
    this.resetList();
    this.loading.dismissDeviceLoading();
    this.filterOptions = this.filterProvider.getFilterOptions();
    this.addFilter();
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
          placeholder: 'Ex: 12345',
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

  loadNotification() {
    this.navCtrl.push(NotificationPage);
  }

  loadLogOut(){
    this.navCtrl.setRoot(LogInPage);
    //this.fcm.removeToken();
    this.auth.signOut();
    this.navCtrl.popToRoot();
  }

  openFilterModal() {
    this.navCtrl.push(FilterPage, this.filterOptions);
  }

  addFilter() {
    this.displayingDevices = [];
    if (this.filterOptions.alive == AliveState.all && this.filterOptions.battery == BatteryState.all) {
      this.resetList();
    } else {
      let temp: Device[] = [];
      if (this.filterOptions.alive == AliveState.alive) {
        for (let i=0; i<this.devices.length; i++) {
          if (this.devices[i].isAlive) {
            temp.push(this.devices[i]);
          }
        }
      } else if (this.filterOptions.alive == AliveState.dead) {
        for (let i=0; i<this.devices.length; i++) {
          if (!this.devices[i].isAlive) {
            temp.push(this.devices[i]);
          }
        }
      } else {
        console.log('reset');
        temp = this.devices.slice();
      }
      if (this.filterOptions.battery == BatteryState.high) {
        for (let i=0; i<temp.length; i++) {
          if (temp[i].lastLog.battery > 20) {
            this.displayingDevices.push(temp[i]);
          }
        }
      } else if (this.filterOptions.battery == BatteryState.low) {
        for (let i=0; i<temp.length; i++) {
          if (temp[i].lastLog.battery <= 20) {
            this.displayingDevices.push(temp[i]);
          }
        }
      } else {
        this.displayingDevices = temp.slice();
      }
    }
    this.setFilterList();
  }

  setFilterList() {
    this.filterStrings = [];
    if (this.filterOptions.alive === AliveState.alive) {
      this.filterStrings.push('Alive')
    }
    if (this.filterOptions.alive === AliveState.dead) {
      this.filterStrings.push('Dead')
    }
    if (this.filterOptions.battery === BatteryState.high) {
      this.filterStrings.push('High battery')
    }
    if (this.filterOptions.battery === BatteryState.low) {
      this.filterStrings.push('Low battery')
    }
  }

  removeFilter(alive: Boolean) {
    if (alive) {
      this.filterOptions.alive = AliveState.all;
    } else {
      this.filterOptions.battery = BatteryState.all;
    }
    this.filterProvider.setFilterOptions(this.filterOptions);
    this.addFilter();
  }

  resetList() {
    this.displayingDevices = this.devices.slice();
  }

}
