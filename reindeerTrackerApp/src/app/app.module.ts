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
    MapPage
  ],
  imports: [
    BrowserModule,
    IonicModule.forRoot(MyApp),
    BrowserAnimationsModule,
    NgxQRCodeModule
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
    MapPage
  ],
  providers: [
    StatusBar,
    SplashScreen,
    {provide: ErrorHandler, useClass: IonicErrorHandler},
    BarcodeScanner
  ]
})
export class AppModule {}
