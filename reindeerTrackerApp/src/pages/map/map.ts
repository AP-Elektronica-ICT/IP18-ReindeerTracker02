import { Component, ViewChild, ElementRef } from '@angular/core';
import {IonicPage, NavController, NavParams, Platform} from 'ionic-angular';
import { LaunchNavigator} from '@ionic-native/launch-navigator'

/**
 * Generated class for the MapPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

declare var google: any;

@IonicPage()
@Component({
  selector: 'page-map',
  templateUrl: 'map.html',
})
export class MapPage {

  @ViewChild('map') mapRef: ElementRef;

  map: any;
  lat: number;
  long: number;

  constructor(public navCtrl: NavController, public navParams: NavParams, private launchNavigator: LaunchNavigator, private platform: Platform) {
    this.platform.ready().then(() => {
      this.loadMaps();
    })
  }

  loadMaps() {
    this.lat = this.navParams.get('lat');
    this.long = this.navParams.get('long');

    this.showMap();

    this.addRadius(new google.maps.LatLng(this.lat, this.long), this.map);
  }

  showMap(){
    //location - lat long
    const location = new google.maps.LatLng(this.lat, this.long);

    //map options
    const options = {
      center: location,
      zoom : 13,
      streetViewControl: false,
      mapTypeId: 'terrain'
    }

    this.map = new google.maps.Map(this.mapRef.nativeElement, options);
  }

  addRadius(position, map){
    return new google.maps.Circle({
      strokeColor: '#FF0000',
      strokeOpacity: 0.8,
      strokeWeight: 2,
      fillColor: '#FF0000',
      fillOpacity: 0.35,
      map: map,
      center: position,
      radius: 1000
    });
  }

  navMe(){
    const position = this.lat + ',' + this.long;
    this.launchNavigator.navigate(position);
  }

}
