using UnityEngine;
using System.Collections;

public class Shooting : MonoBehaviour {
	public GameObject projectile;
	public float projectileSpeed = 10.0f;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		if(Input.GetKeyDown("space")) {
			
			Movement movement = gameObject.GetComponent<Movement> ();
			float offset = movement.lastDirection * GetComponent<BoxCollider2D>().size.x;
			Vector3 position = transform.position;
			position.x += offset;
			position.y += GetComponent<BoxCollider2D>().size.y;
			GameObject newProjectile = (GameObject) Instantiate(projectile, position, transform.rotation);
			Rigidbody2D body = newProjectile.GetComponent<Rigidbody2D> ();
			Vector2 velocity = new Vector2(movement.lastDirection * projectileSpeed, projectileSpeed);
			body.velocity = velocity;
		}
	}
}
