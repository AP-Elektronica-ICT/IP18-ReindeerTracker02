import { Component } from '@angular/core';
import { Platform } from 'ionic-angular';
import { StatusBar } from '@ionic-native/status-bar';
import { SplashScreen } from '@ionic-native/splash-screen';

import { LogInPage } from '../pages/log-in/log-in';

import * as firebase from 'firebase';
import {firebaseConfig} from "./app.module";
import {AngularFireAuth} from "angularfire2/auth";
import {FcmProvider} from "../providers/fcm/fcm";
import {HomePage} from "../pages/home/home";
import {LoadingProvider} from "../providers/loading/loading";
import {DateformatProvider} from "../providers/dateformat/dateformat";
@Component({
  templateUrl: 'app.html'
})
export class MyApp {
  rootPage:any = LogInPage;

  constructor(platform: Platform, statusBar: StatusBar, splashScreen: SplashScreen, private af: AngularFireAuth, private fcmProvider: FcmProvider, private loading: LoadingProvider) {
    //this.loading.presentLoginLoading();
    platform.ready().then(() => {
      // Okay, so the platform is ready and our plugins are available.
      // Here you can do any higher level native things you might need.
      //firebase.initializeApp(firebaseConfig);
      af.auth.onAuthStateChanged((auth) => {
        console.log(auth, 'auth');
        if (auth) {
          this.rootPage = HomePage;
          this.initializeFCM();
          this.loading.presentDeviceLoading();
        }
        this.loading.dismissLoginLoading();
        statusBar.styleDefault();
        splashScreen.hide();
        console.log('hide');
      })
    });
  }

  private initializeFCM() {
    this.fcmProvider.setFirebaseOnRefreshToken();
    this.fcmProvider.getToken();
    this.fcmProvider.listenToNotifications();
  }
}

