import { Component, ViewChild, ElementRef } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';

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

  constructor(public navCtrl: NavController, public navParams: NavParams) {
  }

  ionViewDidLoad() {
    this.showMap();

    this.addMarker(new google.maps.LatLng(51.058518, 5.271671), this.map);
  }

  showMap(){
    //location - lat long
    const location = new google.maps.LatLng(51.058518, 5.271671);

    //map options
    const options = {
      center: location,
      zoom : 17
    }

    this.map = new google.maps.Map(this.mapRef.nativeElement, options);
  }

  addMarker(position, map){
    return new google.maps.Marker({
      position,
      map
    })
  }

}
