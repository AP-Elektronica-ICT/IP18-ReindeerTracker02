import { Component } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';
import {AliveState, BatteryState, FilterOptions} from "../../classes/filterOptions";
import {HomePage} from "../home/home";
import {FilterProvider} from "../../providers/filter/filter";

/**
 * Generated class for the FilterPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-filter',
  templateUrl: 'filter.html',
})
export class FilterPage {
  options: FilterOptions;
  aliveState = AliveState;
  batteryState = BatteryState;
  callback: any;

  constructor(public navCtrl: NavController, public navParams: NavParams, private filterProvider: FilterProvider) {
    this.options = this.navParams.data;
  }

  ionViewDidLoad() {

  }

  save() {
    console.log(this.options);
    this.filterProvider.setFilterOptions(this.options);
    this.navCtrl.pop();
  }

  reset() {
    this.options.alive = AliveState.all;
    this.options.battery = BatteryState.all;
  }

}
