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
  filter: FilterOptions = null;
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
    /*this.deviceProvider.getUserDevices()
      .subscribe(res => {
        this.devices = res;
        this.displayingDevices = res;
        console.log(this.devices);
        this.loading.dismissDeviceLoading();
        this.filter = this.filterProvider.getFilterOptions();
        this.addFilter();

      })*/
    this.deviceProvider.getUserDevicesFromStorage()
      .then(devices => {
        this.afterDevicesLoaded(devices);
      })
  }

  afterDevicesLoaded(devices: Device[]) {
    this.devices = devices;
    this.resetList();
    this.loading.dismissDeviceLoading();
    this.filter = this.filterProvider.getFilterOptions();
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
    this.fcm.removeToken();
    this.auth.signOut();
    this.navCtrl.popToRoot();
  }

  openFilterModal() {
    this.navCtrl.push(FilterPage, this.filter);
  }

  addFilter() {
    if (this.filter.alive === AliveState.all && this.filter.battery === BatteryState.all) {
      console.log('reset filter');
      this.resetList();
    } else {
      console.log('applying filter');
      this.displayingDevices = [];
      if (this.filter.alive == AliveState.alive) {
        for (let i=0; i<this.devices.length; i++) {
          if (this.devices[i].isAlive) {
            console.log('adding: ' + this.devices[i]);
            this.displayingDevices.push(this.devices[i]);
          }
        }
      } else if (this.filter.alive == AliveState.dead) {
        for (let i=0; i<this.devices.length; i++) {
          if (!this.devices[i].isAlive) {
            this.displayingDevices.push(this.devices[i]);
          }
        }
      } else {
        this.resetList();
      }
      console.log(this.displayingDevices);
      for (let i=0; i<this.displayingDevices.length; i++) {
        if (this.filter.battery !== BatteryState.all) {
          for (let i=0; i<this.displayingDevices.length; i++) {
            if (this.filter.battery === BatteryState.high && this.displayingDevices[i].lastLog.battery <= 20) {
              this.displayingDevices.splice(i, 1);
            } else if (this.filter.battery === BatteryState.low && this.displayingDevices[i].lastLog.battery > 20) {
              this.displayingDevices.splice(i, 1);
            }
          }
        }
      }
    }
    this.setFilterList();
  }

  setFilterList() {
    this.filterStrings = [];
    if (this.filter.alive === AliveState.alive) {
      this.filterStrings.push('Alive')
    }
    if (this.filter.alive === AliveState.dead) {
      this.filterStrings.push('Dead')
    }
    if (this.filter.battery === BatteryState.high) {
      this.filterStrings.push('High battery')
    }
    if (this.filter.battery === BatteryState.low) {
      this.filterStrings.push('Low battery')
    }
  }

  removeFilter(alive: Boolean) {
    if (alive) {
      this.filter.alive = AliveState.all;
    } else {
      this.filter.battery = BatteryState.all;
    }
    this.filterProvider.setFilterOptions(this.filter);
    this.addFilter();
  }

  resetList() {
    this.displayingDevices = this.devices.slice();
  }

}
