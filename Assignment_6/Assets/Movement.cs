using UnityEngine;
using System.Collections;

public class Movement : MonoBehaviour {
	public float speed = 10.0f;
	public float jumpSpeed = 20.0f;
	private bool colliding;
	private bool grounded = false;
	public float lastDirection = 1;
	public Camera camera;

	// Use this for initialization
	void Start () {
		colliding = false;
		grounded = false;
	}
		
	void OnCollisionStay2D(Collision2D coll) {
		Rigidbody2D body = GetComponent<Rigidbody2D> ();
		bool points = false;
		foreach (ContactPoint2D point in coll.contacts) {
			points = true;
			if (point.point.y > body.position.y - GetComponent<BoxCollider2D>().size.y / 2) {
				points = false;
				break;
			}
		}
		grounded = points || grounded;
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
		float direction = Input.GetAxisRaw ("Horizontal");
		float xVelocity = direction * speed;
		velocity.x = xVelocity;

		if (direction != 0) {
			lastDirection = direction;
		}

		if (Input.GetAxisRaw ("Vertical") == 1 && grounded) {
			velocity.y = jumpSpeed;
		}
		body.velocity = velocity;

	}

	void LateUpdate() {
		Vector3 cameraPos = transform.position;
		cameraPos.z = -10;
		camera.transform.position = cameraPos;
	}
}
