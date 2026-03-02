import { Routes } from '@angular/router';
import { Intro } from './components/intro/intro';
import { Home } from './components/home/home';
import { DummyRefresh } from './components/dummyRefresh/dummy-refresh/dummy-refresh';

export const routes: Routes = [
    {
        path: '',
        component: Intro
    },
    {
        path: 'home',
        component: Home
    },
    {path: '**', component: DummyRefresh},
];