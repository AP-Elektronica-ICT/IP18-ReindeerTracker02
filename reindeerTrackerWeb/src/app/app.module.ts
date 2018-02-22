import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { AngularFireModule } from 'angularfire2';
import { AngularFireDatabaseModule } from 'angularfire2/database';
import { AngularFireAuthModule } from 'angularfire2/auth';

import { AppComponent } from './app.component';
import {environment} from '../environments/environment';
import { NavbarComponent } from './navbar/navbar.component';
import { HomeComponent } from './home/home.component';
import { InfoComponent } from './home/info/info.component';
<<<<<<< HEAD
import { SensorDataComponent } from './sensor-data/sensor-data.component';
=======
import { NewDeviceComponent } from './manifacturer/new-device/new-device.component';
import { ManifacturerComponent } from './manifacturer/manifacturer.component';
import {AppRoutingModule} from "./app-routing.module";
import { ManifacturerHomeComponent } from './manifacturer/manifacturer-home/manifacturer-home.component';
>>>>>>> c85279821ff0a5b5e16b92c63e71c40757cb5a44


@NgModule({
  declarations: [
    AppComponent,
    NavbarComponent,
    HomeComponent,
    InfoComponent,
<<<<<<< HEAD
    SensorDataComponent
=======
    NewDeviceComponent,
    ManifacturerComponent,
    ManifacturerHomeComponent
>>>>>>> c85279821ff0a5b5e16b92c63e71c40757cb5a44
  ],
  imports: [
    BrowserModule,
    AngularFireModule.initializeApp(environment.firebase),
    AngularFireDatabaseModule,
    AngularFireAuthModule,
    AppRoutingModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
