import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import {AngularFireModule, FirebaseApp} from 'angularfire2';
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
import {FormsModule, ReactiveFormsModule} from "@angular/forms";
import {HttpModule} from "@angular/http";
import {HttpClientModule} from "@angular/common/http";
import { AddDeviceComponent } from './add-device/add-device.component';
import {AuthService} from "./shared/auth.service";
import { DeviceInfoComponent } from './device-info/device-info.component';
import {KeysService} from "./shared/keys.service";
import {DeviceService} from "./shared/device.service";
import {StorageService} from "./shared/storage.service";
import { Http, Response } from "@angular/http";
import { DetailComponent } from './detail/detail.component';
import { AgmCoreModule } from '@agm/core';
import { LoginComponent } from './login/login.component';
import { SignupComponent } from './signup/signup.component';
import { PasswordResetComponent } from './password-reset/password-reset.component';
import { ProfileComponent } from './profile/profile.component';
import { ProfileEditComponent } from './profile-edit/profile-edit.component';
import { NotificationComponent } from './notification/notification.component';



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
    DeviceInfoComponent,
    DetailComponent,
    LoginComponent,
    SignupComponent,
    PasswordResetComponent,
    ProfileComponent,
    ProfileEditComponent,
    NotificationComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpClientModule,
    AngularFireModule.initializeApp(environment.firebase),
    AngularFireDatabaseModule,
    AngularFireAuthModule,
    AppRoutingModule,
    ReactiveFormsModule,
    AgmCoreModule.forRoot({
      apiKey: 'AIzaSyCUwftTaruBNv0xSVOsRaAZRdIsJ6WQhSQ'
    })
  ],
  providers: [AuthService, KeysService, DeviceService, StorageService, AuthService, Http],
  bootstrap: [AppComponent]
})
export class AppModule { }
