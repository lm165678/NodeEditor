#include "Renderable3DQuoteFrame.h"
#include "GraphManager.h"
#include "MathElegent.h"
#include <math.h>
#include "Interface.h"

// Change: 20 vertices to 23 vertices

static const int VertexNumber = 23;

Renderable3DQuoteFrame::Renderable3DQuoteFrame(float width, float height, bool border, float* bcolor, float* acolor, float cornersize, bool d, float inOpacity)
	:Renderable(Programs::ShapeProgram, GraphManager::canvas), bBorder(border), bDynamic(d), opacity(inOpacity)
{
	// Save Data
	memcpy(borderColor, bcolor, sizeof(float) * 3);
	memcpy(areaColor, acolor, sizeof(float) * 3);

	UpdateBuffer(width, height, cornersize);

	// Resize a bit
	float borderWidth = 0.02f;	// 2cm // __Debug_Temp: Pending making this a static member
	float scaleX = (borderWidth / width) + 1;
	float scaleY = (borderWidth / height) + 1;
	// Make things precise
	modelResize = glm::scale(glm::mat4(1), glm::vec3(scaleX, scaleY, 1));	// Resize
	modelResize = glm::translate(modelResize, glm::vec3(-width * ((scaleX - 1) / 2), +height*((scaleY - 1) / 2), -0.001));	// Offset a bit, just a tiny bit
}

// Might be too many draw calls, might want to condense
void Renderable3DQuoteFrame::Render()
{
	// Set up states
	glBindVertexArray(VAO);
	GraphManager::UseNewProgram(program);
	// Set common uniforms
	glUniform1f(3, opacity);	// time scale

	// Draw Border First
	if (bBorder)
	{
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world*modelResize));	// MVP
		glUniform3fv(2, 1, borderColor);	// color
		glDrawArrays(GL_TRIANGLE_FAN, 0, VertexNumber);
	}

	// Draw Face
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr((parentInterface->GetProjectMatrix())*(parentInterface->GetViewMatrix())*world));	// MVP
	glUniform3fv(2, 1, areaColor);	// color
	// Render the image quad
	glDrawArrays(GL_TRIANGLE_FAN, 0, VertexNumber);
}

void Renderable3DQuoteFrame::UpdateBuffer(float width, float height, float cornersize)
{
	// Clamp Border Size
	float smallestHalfDimension = Minf(width / 2, height / 2);
	float radius = Minf(cornersize, smallestHalfDimension);

	// Prepare Data, CW
	// We divide each corner into 4 sections: 0d, 22.5d, 45d, 67.5d, 90d
	// Use a drawing to help visualize vertices
	float	dsR[6] =		// stand for deltaS, in reverse direction
	{
		// Radius should be positive, so all the value calculated are still positive
		radius - radius* cos(0.392f),// d22.5dx
		radius - radius* sin(0.392f),// d22.5dy
		radius - radius* cos(0.785f),// d45dx
		radius - radius* sin(0.785f),// d45dy
		radius - radius* cos(1.178f),// d67.5dx
		radius - radius* sin(1.178f),// d67.5dy
	};
	float vertexData[VertexNumber * 3] =
	{
		// Corner UL
		0, -radius, 0,
		dsR[0], -dsR[1], 0,
		dsR[2], -dsR[3], 0,
		dsR[4], -dsR[5], 0,
		radius, 0, 0,
		// Corner UR
		width - radius, 0, 0,
		width - dsR[4], -dsR[5], 0,	// Notice in a reverse order compared with the first(upper-left) corner
		width - dsR[2], -dsR[3], 0,
		width - dsR[0], -dsR[1], 0,
		width, -radius, 0,
		// Coner LR
		width, -height + radius, 0,	// Remember in openGL second quadrant
		width - dsR[0], -height + dsR[1], 0,
		width - dsR[2], -height + dsR[3], 0,
		width - dsR[4], -height + dsR[5], 0,
		width - radius, -height, 0,
		// Quote Symbol
		width - radius - 0.32*width, -height, 0,
		width - radius - 0.28*width, -height - 0.26 * height, 0,
		width - radius - 0.42*width, -height, 0,
		// Connoer LL
		radius, -height, 0,
		dsR[4], -height + dsR[5], 0,
		dsR[2], -height + dsR[3], 0,
		dsR[0], -height + dsR[1], 0,
		0, -height + radius, 0
	};

	// Initialize OpenGL obejcts
	glBindVertexArray(VAO);
	// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, VertexNumber * 3 * sizeof(float), vertexData, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);	// Might by dynamic
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);	// Adhering to ShapeProgram
	glEnableVertexAttribArray(0);
}

void Renderable3DQuoteFrame::UpdateProperty(bool border, float* bcolor, float* acolor, float inOpacity)
{
	opacity = inOpacity;
	memcpy(borderColor, bcolor, sizeof(float) * 3);
	memcpy(areaColor, acolor, sizeof(float) * 3);
	bBorder = border;
}