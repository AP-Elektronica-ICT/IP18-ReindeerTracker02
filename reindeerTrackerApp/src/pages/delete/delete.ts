import { Component } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';
import {DeviceProvider} from "../../providers/device/device";

/**
 * Generated class for the DeletePage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-delete',
  templateUrl: 'delete.html',
})
export class DeletePage {
  devices: any = null;
  selectedDevices = [];
  deleteCount = 0;

  constructor(public navCtrl: NavController, public navParams: NavParams, private deviceProvider: DeviceProvider) {
    this.getDevices();
  }

  getDevices() {
    this.deviceProvider.getUserDevices()
      .subscribe(res => {
        this.devices = res;
        console.log(this.devices);
      })
  }

  toggleDevice(deviceKey: string) {
    const index = this.selectedDevices.indexOf(deviceKey);
    if (index > -1) {
      this.selectedDevices.splice(index, 1);
    } else {
      this.selectedDevices.push(deviceKey);
    }
    console.log(this.selectedDevices);
  }

  deleteSelectedDevices() {
    const amout = this.selectedDevices.length;
    for (let i = 0; i < amout; i++) {
      this.deviceProvider.removeDeviceFromUser(this.selectedDevices[i])
        .subscribe(res => {
          this.deleteCount++;
          if (this.deleteCount == amout) {
            this.deviceProvider.loadUserDevices()
              .then(() => {
                this.navCtrl.pop();
              });
          }
        })
    }
  }

  ionViewDidLoad() {
    console.log('ionViewDidLoad DeletePage');
  }

}
