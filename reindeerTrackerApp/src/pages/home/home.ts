import { Component } from '@angular/core';
import { NavController } from 'ionic-angular';
import { DetailPage } from '../detail/detail';
import { DeletePage } from '../delete/delete';
import { AddPage } from '../add/add';
import { ProfilePage } from '../profile/profile';
import { LogInPage } from '../log-in/log-in';

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {

  constructor(public navCtrl: NavController) {

  }

  loadDetail(){
    this.navCtrl.push(DetailPage);
  }

  loadDelete(){
    this.navCtrl.push(DeletePage);
  }

  loadAdd(){
    this.navCtrl.push(AddPage);
  }

  loadProfile(){
    this.navCtrl.push(ProfilePage);
  }

  loadLogOut(){
    this.navCtrl.push(LogInPage);
  }

}
