using UnityEngine;
using System.Collections;

public class Movement : MonoBehaviour {
	public float speed = 10.0f;
	public float jumpSpeed = 5.0f;

	// Use this for initialization
	void Start () {
	}

	// Update is called once per frame
	void FixedUpdate () {
		Rigidbody2D body = GetComponent<Rigidbody2D> ();
		Vector2 velocity = body.velocity;
		float xVelocity = Input.GetAxisRaw ("Horizontal") * speed;
		velocity.x = xVelocity;

		if (Input.GetAxisRaw ("Vertical") == 1) {
			velocity.y = jumpSpeed;
		}
		body.velocity = velocity;

	}
}
