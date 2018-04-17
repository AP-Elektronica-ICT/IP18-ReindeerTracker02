import { Component } from '@angular/core';
import { IonicPage, NavController, NavParams } from 'ionic-angular';
import {FormGroup} from "@angular/forms";
import {Userdata} from "../../classes/userdata";
import {AuthProvider} from "../../providers/auth/auth";

/**
 * Generated class for the SignUpPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-sign-up',
  templateUrl: 'sign-up.html',
})
export class SignUpPage {

  currentUser: Userdata = null;
  loaded = false;
  fromProfile = false;
  errorMessage = null;
  locationArr = ["Käsivarsi", "Kemin-Sompio", "Kiiminki", "Kolari", "Kollaja", "Kuivasalmi", "Kuukas", "Kyrö", "Lappi", "Lohijärvi", "Muddusjärvi", "Muonio", "Muotkatunturi", "Näätämö", "Näkkälä", "Näljänkä", "Narkaus", "Niemelä", "Oijärvi", "Oivanki", "Orajärvi", "Oraniemi", "Paatsjoki", "Paistunturi", "Palojärvi", "Pintamo", "Pohjois-Salla", "Poikajärvi", "Posion", "Livo", "Pudasjärven", "Livo", "Pudasjärvi", "Pyhä-Kallio", "Salla", "Sallivaara", "Sattasniemi", "Syväjärvi", "Taivalkoski", "Timisjärvi", "Tolva", "Vanttaus", "Vätsäri"]

  constructor(public navCtrl: NavController, public navParams: NavParams, private auth: AuthProvider) {
  }

  ionViewDidLoad() {
    console.log('ionViewDidLoad SignUpPage');
    if (this.navParams.data.firstName) {
      this.currentUser = this.navParams.data;
      this.fromProfile = true;
    } else {
      this.currentUser = {
        uid: '',
        email: '',
        firstName: '',
        lastName: '',
        birthdate: null,
        phoneNumber: '',
        location: ''
      }
    }
    this.loaded = true;
    console.log(this.currentUser);
  }

  signUp(form: FormGroup) {
    const email = form.controls.email.value;
    const password =  form.controls.password.value;
    const passwordrpt = form.controls.passwordrpt.value;
    const userdata = {
      email: email,
      firstName: form.controls.firstName.value,
      lastName: form.controls.lastName.value,
      birthdate: new Date(form.controls.birthdate.value),
      phoneNumber: form.controls.phoneNumber.value,
      location: form.controls.location.value
    };
    if (password === passwordrpt && form.valid) {
      console.log(userdata);
      this.auth.signupWithEmailPassword(email, password, userdata as Userdata)
        .subscribe(res => {
          console.log(res);
          this.navCtrl.pop();
        }, err => {
          this.errorMessage = 'There is already an account with this email address';
        })
    } else if (password !== passwordrpt) {
      this.errorMessage = 'The passwords do not match.';
    } else if (!form.valid) {
      this.errorMessage = 'Please fill in all the required fields';
    }
    console.log(this.errorMessage);
  }

  saveChanges() {
    //TODO: send to server
    this.currentUser.birthdate = new Date(this.currentUser.birthdate);
    console.log(this.currentUser);
  }

}
