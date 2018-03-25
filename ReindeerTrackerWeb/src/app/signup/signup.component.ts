import { Component, OnInit } from '@angular/core';
import {FormGroup} from "@angular/forms";
import {AuthService} from "../shared/auth.service";
import {Userdata} from "../shared/userdata";
import {Router} from "@angular/router";

@Component({
  selector: 'app-signup',
  templateUrl: './signup.component.html',
  styleUrls: ['./signup.component.css']
})
export class SignupComponent implements OnInit {
  errorMessage = null;
  locationArr = ["Käsivarsi", "Kemin-Sompio", "Kiiminki", "Kolari", "Kollaja", "Kuivasalmi", "Kuukas", "Kyrö", "Lappi", "Lohijärvi", "Muddusjärvi", "Muonio", "Muotkatunturi", "Näätämö", "Näkkälä", "Näljänkä", "Narkaus", "Niemelä", "Oijärvi", "Oivanki", "Orajärvi", "Oraniemi", "Paatsjoki", "Paistunturi", "Palojärvi", "Pintamo", "Pohjois-Salla", "Poikajärvi", "Posion", "Livo", "Pudasjärven", "Livo", "Pudasjärvi", "Pyhä-Kallio", "Salla", "Sallivaara", "Sattasniemi", "Syväjärvi", "Taivalkoski", "Timisjärvi", "Tolva", "Vanttaus", "Vätsäri"]

  constructor(private auth: AuthService, private router: Router) {

  }

  ngOnInit() {
  }

  signup(form: FormGroup) {
    this.errorMessage = null;
    const email = form.controls.email.value;
    const password =  form.controls.password.value;
    const passwordrpt = form.controls.passwordrpt.value;
    const userdata = {
      email: email,
      firstName: form.controls.firstName.value,
      lastName: form.controls.lastName.value,
      birthdate: new Date(form.controls.birthyear.value, form.controls.birthmonth.value, form.controls.birthday.value),
      phoneNumber: '+358' + form.controls.phoneNumber.value,
      location: form.controls.location.value
    };
    if (password === passwordrpt && form.valid) {
      console.log(userdata);
      this.auth.signupWithEmailPassword(email, password, userdata as Userdata)
        .subscribe(res => {
          //TODO: go to other page
          console.log(res);
          this.router.navigate(['/']);
        }, err => {
          this.errorMessage = 'There is already an account with this email address';
        })
    } else if (password !== passwordrpt) {
      this.errorMessage = 'The passwords do not match.';
    } else if (!form.valid) {
      this.errorMessage = 'Please fill in all the required fields';
    }
  }

}
