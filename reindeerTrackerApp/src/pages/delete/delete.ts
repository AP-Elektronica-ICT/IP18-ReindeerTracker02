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

  ionViewDidLoad() {
    console.log('ionViewDidLoad DeletePage');
  }

}
