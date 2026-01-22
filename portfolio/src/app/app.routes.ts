import { Routes } from '@angular/router';
import { Home } from './components/home/home';
import { About } from './components/about/about';
import { Projects } from './components/projects/projects';
import { Goals } from './components/goals/goals';

export const routes: Routes = [
    {
        path: '',
        component: Home
    },
    {
        path: 'about',
        component: About
    },
    {
        path: 'projects',
        component: Projects
    },
    {
        path: 'goals',
        component: Goals
    }
];