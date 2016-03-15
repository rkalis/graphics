using UnityEngine;
using System.Collections;

public class Movement : MonoBehaviour {
	public float speed = 10.0f;
	public float jumpSpeed = 5.0f;
	private bool colliding;
	private bool grounded;

	// Use this for initialization
	void Start () {
		colliding = false;
		grounded = false;
	}
		
	void OnCollisionStay2D(Collision2D coll) {
		Rigidbody2D body = GetComponent<Rigidbody2D> ();
		if (coll.contacts [0].point.y < body.position.y &&
			coll.contacts [1].point.y < body.position.y) {
			grounded = true;
		}
		colliding = true;
	}
	void OnCollisionExit2D(Collision2D coll) {
		colliding = false;
		grounded = false;
	}

	// Update is called once per frame
	void FixedUpdate () {
		Rigidbody2D body = GetComponent<Rigidbody2D> ();
		Vector2 velocity = body.velocity;
		float xVelocity = Input.GetAxisRaw ("Horizontal") * speed;
		velocity.x = xVelocity;

		if (Input.GetAxisRaw ("Vertical") == 1 && grounded) {
			velocity.y = jumpSpeed;
		}
		body.velocity = velocity;

	}
}
