import { BrowserModule } from '@angular/platform-browser';
import { ErrorHandler, NgModule } from '@angular/core';
import { IonicApp, IonicErrorHandler, IonicModule } from 'ionic-angular';
import { SplashScreen } from '@ionic-native/splash-screen';
import { StatusBar } from '@ionic-native/status-bar';

import { MyApp } from './app.component';
import { HomePage } from '../pages/home/home';
import { DetailPage } from '../pages/detail/detail';
import { DeletePage } from '../pages/delete/delete';
import { AddPage } from '../pages/add/add';
import { ProfilePage } from '../pages/profile/profile';
import { LogInPage } from '../pages/log-in/log-in';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { SignUpPage } from '../pages/sign-up/sign-up';
import { NgxQRCodeModule } from 'ngx-qrcode2';
import { BarcodeScanner } from '@ionic-native/barcode-scanner';
import { MapPage } from '../pages/map/map';
import { DeviceProvider } from '../providers/device/device';
import {HttpClientModule} from "@angular/common/http";
import {ImagePicker} from "@ionic-native/image-picker";
import {AndroidPermissions} from "@ionic-native/android-permissions";
import {AngularFireModule} from "angularfire2";
import { StorageProvider } from '../providers/storage/storage';
import { ImageProvider } from '../providers/image/image';
import { PreloaderProvider } from '../providers/preloader/preloader';
import {Camera} from "@ionic-native/camera";
import { AuthProvider } from '../providers/auth/auth';
import {AngularFireAuthModule} from "angularfire2/auth";
import {FcmProvider} from "../providers/fcm/fcm";
import {Firebase} from "@ionic-native/firebase";
import {LaunchNavigator} from "@ionic-native/launch-navigator";
import { LoadingProvider } from '../providers/loading/loading';
import {FilterPage} from "../pages/filter/filter";
import { FilterProvider } from '../providers/filter/filter';
import { NotificationProvider } from '../providers/notification/notification';
import {NotificationPage} from "../pages/notification/notification";
import { LinkProfelPage } from '../pages/link-profel/link-profel';

export const firebaseConfig = {
  apiKey: "AIzaSyAud_cLzms8_U55nCkSKNmnRUhXei2pLQs",
  authDomain: "reindeer-tracker.firebaseapp.com",
  databaseURL: "https://reindeer-tracker.firebaseio.com/",
  projectId: "reindeer-tracker",
  storageBucket: "reindeer-tracker.appspot.com",
  messagingSenderId: "771165411039"
};

@NgModule({
  declarations: [
    MyApp,
    HomePage,
    DetailPage,
    DeletePage,
    AddPage,
    ProfilePage,
    LogInPage,
    SignUpPage,
    MapPage,
    FilterPage,
    NotificationPage,
    LinkProfelPage
  ],
  imports: [
    BrowserModule,
    IonicModule.forRoot(MyApp),
    BrowserAnimationsModule,
    NgxQRCodeModule,
    HttpClientModule,
    AngularFireModule.initializeApp(firebaseConfig),
    AngularFireAuthModule
  ],
  bootstrap: [IonicApp],
  entryComponents: [
    MyApp,
    HomePage,
    DetailPage,
    DeletePage,
    AddPage,
    ProfilePage,
    LogInPage,
    SignUpPage,
    MapPage,
    FilterPage,
    NotificationPage,
    LinkProfelPage
  ],
  providers: [
    StatusBar,
    SplashScreen,
    {provide: ErrorHandler, useClass: IonicErrorHandler},
    BarcodeScanner,
    DeviceProvider,
    ImagePicker,
    AndroidPermissions,
    StorageProvider,
    ImageProvider,
    PreloaderProvider,
    Camera,
    AuthProvider,
    FcmProvider,
    Firebase,
    LaunchNavigator,
    LoadingProvider,
    FilterProvider,
    NotificationProvider
  ]
})
export class AppModule {}
