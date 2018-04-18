import { Component, trigger, state, style, transition, animate,keyframes } from '@angular/core';
import {AlertController, IonicPage, NavController, NavParams} from 'ionic-angular';
import { HomePage } from '../home/home';
import { SignUpPage } from '../sign-up/sign-up';
import {FormGroup} from "@angular/forms";
import {AuthProvider} from "../../providers/auth/auth";
import {LoadingProvider} from "../../providers/loading/loading";
import {DeviceProvider} from "../../providers/device/device";

/**
 * Generated class for the LogInPage page.
 *
 * See https://ionicframework.com/docs/components/#navigation for more info on
 * Ionic pages and navigation.
 */

@IonicPage()
@Component({
  selector: 'page-log-in',
  templateUrl: 'log-in.html',

  animations: [

    //For the logo
    trigger('flyInBottomSlow', [
      state('in', style({
        transform: 'translate3d(0,0,0)'
      })),
      transition('void => *', [
        style({transform: 'translate3d(0,2000px,0'}),
        animate('2000ms ease-in-out')
      ])
    ]),

    //For the background detail
    trigger('flyInBottomFast', [
      state('in', style({
        transform: 'translate3d(0,0,0)'
      })),
      transition('void => *', [
        style({transform: 'translate3d(0,2000px,0)'}),
        animate('1000ms ease-in-out')
      ])
    ]),

    //For the login form
    trigger('bounceInBottom', [
      state('in', style({
        transform: 'translate3d(0,0,0)'
      })),
      transition('void => *', [
        animate('2000ms 200ms ease-in', keyframes([
          style({transform: 'translate3d(0,2000px,0)', offset: 0}),
          style({transform: 'translate3d(0,-20px,0)', offset: 0.9}),
          style({transform: 'translate3d(0,0,0)', offset: 1})
        ]))
      ])
    ]),
    //For login button
    trigger('fadeIn', [
      state('in', style({
        opacity: 1
      })),
      transition('void => *', [
        style({opacity: 0}),
        animate('1000ms 2000ms ease-in')
      ])
    ])
  ]
})
export class LogInPage {

  logoState: any = "in";
  cloudState: any = "in";
  loginState: any = "in";
  formState: any = "in";

  loginError = false;

  constructor(public navCtrl: NavController, public navParams: NavParams, private auth: AuthProvider, public alertCtrl: AlertController, private loading: LoadingProvider, private deviceProvider: DeviceProvider) {
  }

  ionViewDidLoad() {
    console.log(this.auth.isAuthenticated());
    if (this.auth.isAuthenticated()) {
      this.navCtrl.setRoot(HomePage);
    }
  }

  login(form: FormGroup) {
    this.loginError = false;
    const email = form.controls.email.value;
    const password = form.controls.password.value;
    this.auth.loginWithEmailPassword(email, password)
      .then(res => {
        //this.navCtrl.push(HomePage);
        this.loading.presentDeviceLoading();
        this.deviceProvider.loadUserDevices()
          .then(() => {
            this.navCtrl.setRoot(HomePage);
          })
      })
      .catch(err => {
        this.loginError = true;
      })
  }

  signup(){
    this.navCtrl.push(SignUpPage);
  }

  showForgotPasswordPrompt() {
    let prompt = this.alertCtrl.create({
      title: 'Forgot password',
      message: "Please enter your email address, an email to reset your password will be sent shortly after.",
      inputs: [
        {
          name: 'email',
          placeholder: 'Email',
          type: 'email'
        },
      ],
      buttons: [
        {
          text: 'Cancel',
          handler: data => {
            console.log('Cancel clicked');
          }
        },
        {
          text: 'Send',
          handler: data => {
            this.auth.resetPassword(data.email)
              .then(res => {
                this.showEmailResetAlert();
              })
          }
        }
      ]
    });
    prompt.present();
  }

  showEmailResetAlert() {
    let alert = this.alertCtrl.create({
      title: 'Email sent',
      subTitle: 'An email has been sent to reset your password',
      buttons: ['OK']
    });
    alert.present();
  }
}
