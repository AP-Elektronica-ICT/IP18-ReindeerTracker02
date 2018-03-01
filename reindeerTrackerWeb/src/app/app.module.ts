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
import { SensorDataComponent } from './sensor-data/sensor-data.component';
import { NewDeviceComponent } from './manifacturer/new-device/new-device.component';
import { ManifacturerComponent } from './manifacturer/manifacturer.component';
import {AppRoutingModule} from "./app-routing.module";
import { ManifacturerHomeComponent } from './manifacturer/manifacturer-home/manifacturer-home.component';
import {FormsModule} from "@angular/forms";
import {HttpModule} from "@angular/http";
import {HttpClientModule} from "@angular/common/http";
import { AddDeviceComponent } from './add-device/add-device.component';
import {AuthService} from "./shared/auth.service";
import { DeviceInfoComponent } from './device-info/device-info.component';


@NgModule({
  declarations: [
    AppComponent,
    NavbarComponent,
    HomeComponent,
    InfoComponent,
    SensorDataComponent,
    NewDeviceComponent,
    ManifacturerComponent,
    ManifacturerHomeComponent,
    AddDeviceComponent,
    DeviceInfoComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpClientModule,
    AngularFireModule.initializeApp(environment.firebase),
    AngularFireDatabaseModule,
    AngularFireAuthModule,
    AppRoutingModule
  ],
  providers: [AuthService],
  bootstrap: [AppComponent]
})
export class AppModule { }
