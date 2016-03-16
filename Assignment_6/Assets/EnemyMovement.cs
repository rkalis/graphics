using UnityEngine;
using System.Collections;

public class EnemyMovement : MonoBehaviour {
	public float speed = 5.0f;
	public int direction = 1;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void FixedUpdate () {
		Rigidbody2D body = GetComponent<Rigidbody2D> ();
		Vector2 velocity = body.velocity;
		float xVelocity = direction * speed;
		velocity.x = xVelocity;
		body.velocity = velocity;
	}
}
