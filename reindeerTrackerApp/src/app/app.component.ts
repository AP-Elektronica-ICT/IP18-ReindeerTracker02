import { Component } from '@angular/core';
import { Platform } from 'ionic-angular';
import { StatusBar } from '@ionic-native/status-bar';
import { SplashScreen } from '@ionic-native/splash-screen';

import { LogInPage } from '../pages/log-in/log-in';

import * as firebase from 'firebase';
import {firebaseConfig} from "./app.module";
import {AngularFireAuth} from "angularfire2/auth";
@Component({
  templateUrl: 'app.html'
})
export class MyApp {
  rootPage:any = LogInPage;

  constructor(platform: Platform, statusBar: StatusBar, splashScreen: SplashScreen, private af: AngularFireAuth) {
    platform.ready().then(() => {
      // Okay, so the platform is ready and our plugins are available.
      // Here you can do any higher level native things you might need.
      //firebase.initializeApp(firebaseConfig);
      af.auth.onAuthStateChanged((auth) => {
        console.log(auth);
        statusBar.styleDefault();
        splashScreen.hide();
        console.log('hide');
      })
    });
  }
}

