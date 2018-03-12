import { Component, OnInit } from '@angular/core';
import {FormGroup} from "@angular/forms";
import {AuthService} from "../shared/auth.service";

@Component({
  selector: 'app-password-reset',
  templateUrl: './password-reset.component.html',
  styleUrls: ['./password-reset.component.css']
})
export class PasswordResetComponent implements OnInit {
  showSuccessMessage = false;
  showErrorMessage = false;
  email = '';

  constructor(private auth: AuthService) { }

  ngOnInit() {
  }

  sendEmail(form: FormGroup) {
    this.showSuccessMessage = false;
    this.showErrorMessage = false;
    const email = form.controls.email.value;
    if (form.valid) {
      this.email = email;
      this.auth.resetPassword(email)
        .then(res => {
          form.controls.email.setValue('');
          this.showSuccessMessage = true;
        })
        .catch(err => {
          this.showErrorMessage = true;
        })
    }
  }

}
