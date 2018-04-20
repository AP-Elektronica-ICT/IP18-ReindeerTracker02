import {HomeComponent} from "./home/home.component";
import {ManifacturerComponent} from "./manifacturer/manifacturer.component";
import {NewDeviceComponent} from "./manifacturer/new-device/new-device.component";
import {RouterModule} from "@angular/router";
import {NgModule} from "@angular/core";
import {ManifacturerHomeComponent} from "./manifacturer/manifacturer-home/manifacturer-home.component";
import {AddDeviceComponent} from "./add-device/add-device.component";
import {DeviceInfoComponent} from "./device-info/device-info.component";
import {LoginComponent} from "./login/login.component";
import {SignupComponent} from "./signup/signup.component";
import {PasswordResetComponent} from "./password-reset/password-reset.component";
import {SensorDataComponent} from "./sensor-data/sensor-data.component";
import {DetailComponent} from "./detail/detail.component";
import {ProfileComponent} from "./profile/profile.component";
import {ProfileEditComponent} from "./profile-edit/profile-edit.component";

const appRoutes = [
  { path: '', component: HomeComponent},
  { path: 'new-device', component: AddDeviceComponent},
  { path: 'device-info', component: DeviceInfoComponent},
  { path: 'user', component: SensorDataComponent},
  { path: 'detail', component: DetailComponent},
  { path: 'login', component: LoginComponent},
  { path: 'signup', component: SignupComponent},
  { path: 'profile', component: ProfileComponent},
  { path: 'profile/edit', component: ProfileEditComponent},
  { path: 'reset-password', component: PasswordResetComponent},
  { path: 'manifacturer', component: ManifacturerComponent, children: [
      { path: '', component: ManifacturerHomeComponent},
      { path: 'new-device', component: NewDeviceComponent}
    ]}
];

@NgModule({
  imports: [
    RouterModule.forRoot(appRoutes, {useHash: true})
  ],
  exports: [RouterModule]
})
export class AppRoutingModule {

}
