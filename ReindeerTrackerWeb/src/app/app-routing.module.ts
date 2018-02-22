import {HomeComponent} from "./home/home.component";
import {ManifacturerComponent} from "./manifacturer/manifacturer.component";
import {NewDeviceComponent} from "./manifacturer/new-device/new-device.component";
import {RouterModule} from "@angular/router";
import {NgModule} from "@angular/core";
import {ManifacturerHomeComponent} from "./manifacturer/manifacturer-home/manifacturer-home.component";

const appRoutes = [
  { path: '', component: HomeComponent},
  { path: 'manifacturer', component: ManifacturerComponent, children: [
      { path: '', component: ManifacturerHomeComponent},
      { path: 'new-device', component: NewDeviceComponent}
    ]}
];

@NgModule({
  imports: [
    RouterModule.forRoot(appRoutes)
  ],
  exports: [RouterModule]
})
export class AppRoutingModule {

}
