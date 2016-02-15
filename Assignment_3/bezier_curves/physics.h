/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ physics.h
 * Description ..... Physics Engine support things
 * Date ............ 22.07.2009
 * Created by ...... Paul Melis
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#ifdef __cplusplus
extern "C" {
#endif

void    setup_physics(void);
void    reset_physics(void);
void    destroy_physics(void);

void    sim_step(float step, float rotate_joint_vel, float hinge_joint1_vel,
            float hinge_joint2_vel, float grabber_joint_vel);
void    draw_objects(void);

#ifdef __cplusplus
}
#endif

#endif
