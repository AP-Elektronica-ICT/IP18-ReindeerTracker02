import { Component } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';
import { MapPage } from '../map/map';
import {DeviceProvider} from "../../providers/device/device";
import {Device} from "../../classes/device";
import { AddPage } from '../add/add';
import { LinkProfelPage } from '../link-profel/link-profel';

/**
 * Generated class for the DetailPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-detail',
  templateUrl: 'detail.html',
})
export class DetailPage {
  device: Device = null;
  deviceKey = '';

  constructor(public navCtrl: NavController, public navParams: NavParams, private deviceProvider: DeviceProvider) {
  }

  ionViewDidLoad() {
    this.deviceKey = this.navParams.data;
    this.getDevice();
  }

  getDevice() {
    this.deviceProvider.getDevice(this.deviceKey)
      .subscribe((details: Device) => {
        this.device = details;
      })
  }

  goToMap(){
    this.navCtrl.push(MapPage, {lat: this.device.lastLog.lat, long: this.device.lastLog.long, name: this.device.name, gender: this.device.gender, lastSignal: this.device.lastLog.date, battery: this.device.lastLog.battery, DA: this.device.isAlive});
  }

  goToEdit(){
    {
      this.navCtrl.push(AddPage, {deviceKey: this.deviceKey});
    }
  }

  goToLink(){
    this.navCtrl.push(LinkProfelPage, {deviceKey: this.deviceKey});
  }

  deleteDevice() {
    this.deviceProvider.removeDeviceFromUser(this.device.deviceKey)
      .subscribe(res => {
        this.deviceProvider.loadUserDevices()
          .then(() => {
            this.navCtrl.pop();
          })
      })
  }
}
