// The file contents for the current environment will overwrite these during build.
// The build system defaults to the dev environment which uses `environment.ts`, but if you do
// `ng build --env=prod` then `environment.prod.ts` will be used instead.
// The list of which env maps to which file can be found in `.angular-cli.json`.

export const environment = {
  production: false,
  firebase: {
    apiKey: 'AIzaSyAud_cLzms8_U55nCkSKNmnRUhXei2pLQs',
    authDomain: 'reindeer-tracker.firebaseapp.com',
    databaseURL: 'https://reindeer-tracker.firebaseio.com',
    projectId: 'reindeer-tracker',
    storageBucket: 'reindeer-tracker.appspot.com',
    messagingSenderId: '771165411039'
  }
};
