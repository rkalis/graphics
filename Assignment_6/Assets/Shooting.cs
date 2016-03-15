using UnityEngine;
using System.Collections;

public class Shooting : MonoBehaviour {
	public GameObject projectile;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		
		if(Input.GetKeyDown("space")) {
			
			Movement movement = gameObject.GetComponent<Movement> ();
			float offset = movement.lastDirection * GetComponent<BoxCollider2D>().size.x / 2;
			print ("offset: " + offset);
			Vector3 position = transform.position;
			position.x += offset;
			Instantiate(projectile, position, transform.rotation);
		}
	}
}
